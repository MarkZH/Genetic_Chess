#include "Players/CECP_Mediator.h"

#include "Moves/Move.h"
#include "Game/Board.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"

CECP_Mediator::CECP_Mediator() : Outside_Player()
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
        std::string move = receive_command();
        if(String::starts_with(move, "usermove"))
        {
            log("got move " + move);
            log("returning " + String::split(move, " ")[1]);
            return String::split(move, " ")[1];
        }
    }
}

std::string CECP_Mediator::name() const
{
    return "CECP Interface Player";
}

void CECP_Mediator::process_game_ending(const Game_Ending_Exception& gee) const
{
    std::string result;
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
