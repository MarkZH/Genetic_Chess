#include "Players/CECP_Mediator.h"

#include <string>
#include <future>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"

class Move;

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"

//! Set up the connection to the outside interface and send configuration data.
//
//! \param local_player The player on the machine. The name of the player gets sent to the interface.
CECP_Mediator::CECP_Mediator(const Player& local_player) : thinking_mode(NO_THINKING)
{
    std::string expected = "protover 2";
    if(receive_cecp_command(false) == expected)
    {
        log("as expected, setting options");
        send_command("feature "
                     "usermove=1 "
                     "sigint=0 "
                     "sigterm=0 "
                     "reuse=0 "
                     "myname=\"" + local_player.name() + "\" "
                     "name=1 "
                     "ping=1 "
                     "done=1");
    }
    else
    {
        log("ERROR: Expected \"" + expected + "\"");
        throw std::runtime_error("Error in communicating with CECP program.");
    }
}

//! Send the last move the local player made to the interface and wait for the outside player's move.
//
//! \param board The last move on this board is sent to the interface.
//! \param clock The game clock is also adjusted to better match the interface's clock.
//! \throws Game_Ended If the interface sends the "quit" command.
const Move& CECP_Mediator::choose_move(const Board& board, const Clock& clock) const
{
    while(true)
    {
        try
        {
            if(first_move.empty())
            {
                send_command("move " + board.last_move_coordinates());
                received_move_text = receive_move(clock);
            }
            else
            {
                received_move_text = first_move;
                first_move.clear();
            }

            board.set_thinking_mode(thinking_mode);
            return board.create_move(received_move_text);
        }
        catch(const Illegal_Move& e)
        {
            send_command("Illegal move (" + std::string(e.what()) + ") " + received_move_text);
        }
    }
}

Color CECP_Mediator::ai_color() const
{
    while(true)
    {
        auto cmd = receive_cecp_command(false);
        if(cmd == "white" || cmd == "go")
        {
            indent = "\t\t";
            return WHITE;
        }
        else if(String::starts_with(cmd, "usermove"))
        {
            first_move = String::split(cmd, " ")[1];
            indent = "\t\t\t";
            return BLACK;
        }
    }
}

std::string CECP_Mediator::receive_move(const Clock& clock) const
{
    while(true)
    {
        auto move = receive_cecp_command(false);
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
            auto data = String::split(String::split(move, "{", 1)[1], "}", 1)[0];
            throw Game_Ended(winner, data);
        }
        else if(String::starts_with(move, "time"))
        {
            // Waiting for non-local move, so the non-local clock is running
            // for the local AI. The local clock has not been punched yet, so
            // it is still running for the non-local player. Therefore, the
            // "opponent time" ("otim") refers to the local AI's clock.
            auto player = opposite(clock.running_for());
            auto time = std::stod(String::split(move, " ")[1])/100; // time specified in centiseconds
            clock.set_time(player, time);
            log("setting " + color_text(player) + "'s time to " + std::to_string(time) + " seconds.");
        }
    }
}

void CECP_Mediator::ponder(const Board& board, const Clock& clock) const
{
    last_ponder_command = std::async(std::launch::async, &CECP_Mediator::ponder_method, this, board, clock);
}

//! Reports the name of the outside opponent.
//
//! \returns The opponent's name or "CECP Interface Player" if unknown.
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

void CECP_Mediator::process_game_ending(const Game_Result& ending, const Board& board) const
{
    if(board.last_move_coordinates() != received_move_text)
    {
        send_command("move " + board.last_move_coordinates());
    }

    send_command(ending.game_ending_annotation() + " {" + ending.ending_reason() + "}");

    wait_for_quit();
}

void CECP_Mediator::receive_clock_specs()
{
    while(true)
    {
        auto response = receive_cecp_command(false);
        if(String::starts_with(response, "level"))
        {
            log("got time specs: " + response);
            auto split = String::split(response);

            log("moves to reset clock = " + split[1]);
            set_reset_moves(String::string_to_size_t(split[1]));

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

            log("increment = " + split[3]);
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

    log("done with time specs");
}

std::string CECP_Mediator::receive_cecp_command(bool while_pondering) const
{
    auto last_command = ( ! while_pondering && last_ponder_command.valid()) ? last_ponder_command.get() : std::string{};

    while(true)
    {
        auto command = last_command.empty() ? receive_command() : last_command;
        last_command.clear();

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
        else if(String::starts_with(command, "ping"))
        {
            command[1] = 'o'; // change "ping" to "pong"
            send_command(command);
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

void CECP_Mediator::wait_for_quit() const
{
    try
    {
        while(true)
        {
            receive_cecp_command(false);
        }
    }
    catch(const std::runtime_error&)
    {
    }
}

std::string CECP_Mediator::ponder_method(const Board& board, const Clock&) const
{
    while(true)
    {
        auto command = receive_cecp_command(true);
        if(command == "post")
        {
            log("turning on thinking output for CECP");
            board.set_thinking_mode(CECP);
        }
        else if(command == "nopost")
        {
            log("turning off thinking output for CECP");
            board.set_thinking_mode(NO_THINKING);
        }
        else if(command == "?")
        {
            log("Forcing move choice");
            board.pick_move_now();
        }
        else
        {
            log("Ending Pondering");
            return command;
        }
    }
}
