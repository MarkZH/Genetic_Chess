#include "Players/CECP_Mediator.h"

#include "Moves/Move.h"
#include "Game/Board.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"

CECP_Mediator::CECP_Mediator()
{
    if(receive_command() == "protover 2")
    {
        log("as expected, setting options");
        send_command("feature usermove=1 sigint=0 sigterm=0 reuse=0 done=1");
    }
    else
    {
        log("?????????");
    }
}

const Complete_Move CECP_Mediator::choose_move(const Board& board, const Clock& /*clock*/) const
{
    while(true)
    {
        std::string move_text;
        try
        {
            if(first_move.empty())
            {
                send_command("move " + board.last_move());
                move_text = receive_move();
            }
            else
            {
                move_text = first_move;
                first_move.clear();
            }
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
        auto cmd = receive_command();
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

std::string CECP_Mediator::receive_move() const
{
    while(true)
    {
        auto move = receive_command();
        if(String::starts_with(move, "usermove"))
        {
            auto data = String::split(move, " ")[1];
            log("got move " + move);
            log("returning " + data);
            return data;
        }
        else if(String::starts_with(move, "result"))
        {
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
    }
}

std::string CECP_Mediator::name() const
{
    return "CECP Interface Player";
}

void CECP_Mediator::process_game_ending(const Game_Ending_Exception& gee, const Board& board) const
{
    send_command("move " + board.last_move());
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
        auto response = receive_command();
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

            set_increment(std::stof(split[3]));
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
