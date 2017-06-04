#include "Players/CECP_Mediator.h"

#include "Moves/Complete_Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"

CECP_Mediator::CECP_Mediator(const Player& local_player) : thinking_mode(NO_THINKING)
{
    std::string expected = "protover 2";
    if(receive_cecp_command() == expected)
    {
        log("as expected, setting options");
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
                     "sigterm=0 "
                     "reuse=0 "
                     "myname=\"" + local_player.name() + "\" "
                     "name=1 "
                     "done=1");
    }
    else
    {
        log("ERROR: Expected \"" + expected + "\"");
        throw std::runtime_error("Error in communicating with CECP program.");
    }
}

CECP_Mediator::~CECP_Mediator()
{
}

const Complete_Move CECP_Mediator::choose_move(const Board& board, const Clock& clock) const
{
    while(true)
    {
        std::string move_text;
        try
        {
            if(first_move.empty())
            {
                send_command("move " + board.last_move_coordinates());
                move_text = receive_move(clock);
            }
            else
            {
                move_text = first_move;
                first_move.clear();
            }

            board.set_thinking_mode(thinking_mode);
            return board.get_complete_move(move_text);
        }
        catch(const Illegal_Move_Exception& e)
        {
            log("ERROR: Illegal move: " + std::string(e.what()));
            send_command("Illegal move (" + std::string(e.what()) + ") " + move_text);
        }
    }
}

Color CECP_Mediator::get_ai_color() const
{
    while(true)
    {
        auto cmd = receive_cecp_command();
        if(cmd == "white" || cmd == "go")
        {
            return WHITE;
        }
        else if(String::starts_with(cmd, "usermove"))
        {
            first_move = String::split(cmd, " ")[1];
            return BLACK;
        }
    }
}

std::string CECP_Mediator::receive_move(const Clock& clock) const
{
    while(true)
    {
        auto move = receive_cecp_command();
        if(String::starts_with(move, "usermove"))
        {
            auto data = String::split(move, " ")[1];
            log("got move " + move);
            return data;
        }
        else if(String::starts_with(move, "result"))
        {
            log("got result " + move);
            auto winner = NONE;
            if(String::contains(move, "1-0"))
            {
                winner = WHITE;
            }
            else if(String::contains(move, "0-1"))
            {
                winner = BLACK;
            }
            auto data = String::split(move, " ")[1];
            throw Game_Ending_Exception(winner, data);
        }
        else if(String::starts_with(move, "time") || String::starts_with(move, "otim"))
        {
            auto time = std::stod(String::split(move, " ")[1])/100; // time specified in centiseconds

            // Waiting for non-local move, so the non-local clock is running
            // for the local AI. The local clock has not been punched yet, so
            // it is still running for the non-local player. Therefore, the
            // "opponent time" ("otim") refers to the local AI's clock.
            auto player = String::starts_with(move, 'o') ?
                                            clock.running_for() :
                                            opposite(clock.running_for());

            clock.set_time(player, time);
            log("setting " + color_text(player) + "'s time to " + std::to_string(time) + " seconds.");
        }
    }
}

std::string CECP_Mediator::name() const
{
    if(received_name.empty())
    {
        return "CECP Interface Player";
    }
    else
    {
        return received_name;
    }
}

void CECP_Mediator::process_game_ending(const Game_Ending_Exception& gee, const Board& board) const
{
    send_command("move " + board.last_move_coordinates());
    std::string result = "result ";
    if(gee.winner() == WHITE)
    {
        result += "1-0";
    }
    else if(gee.winner() == BLACK)
    {
        result += "0-1";
    }
    else
    {
        result += "1/2-1/2";
    }

    result += " {" + std::string(gee.what()) + "}";

    send_command(result);
}

void CECP_Mediator::get_clock_specs()
{
    while(true)
    {
        auto response = receive_cecp_command();
        if(String::starts_with(response, "level"))
        {
            log("got time specs: " + response);
            auto split = String::split(response);
            set_reset_moves(std::stoi(split[1]));

            auto time_split = String::split(split[2], ":");
            auto game_time = 0;
            if(time_split.size() == 1)
            {
                log("game time = " + time_split[0] + " minutes");
                game_time = 60*std::stoi(time_split[0]);
            }
            else
            {
                log("game time = " + time_split[0] + " minutes and " + time_split[1] + " seconds");
                game_time = 60*std::stoi(time_split[0]) + std::stoi(time_split[1]);
            }
            set_game_time(game_time);

            set_increment(std::stod(split[3]));
            break;
        }
        else if(String::starts_with(response, "st"))
        {
            log("got time specs: " + response);
            auto split = String::split(response);
            auto time_per_move = std::stoi(split[1]);
            set_reset_moves(1);
            set_increment(0);
            set_game_time(time_per_move);
            break;
        }
    }
}

std::string CECP_Mediator::receive_cecp_command() const
{
    while(true)
    {
        auto command = receive_command();
        if(String::starts_with(command, "name"))
        {
            received_name = String::split(command, " ", 1)[1];
            log("got name " + received_name);
        }
        else if(command == "post")
        {
            thinking_mode = CECP;
            log("turning on thinking output for CECP");
        }
        else if(command == "nopost")
        {
            thinking_mode = NO_THINKING;
            log("turning off thinking output for CECP");
        }
        else
        {
            return command;
        }
    }
}

void CECP_Mediator::initial_board_setup(Board& board) const
{
    board.set_thinking_mode(thinking_mode);
}
