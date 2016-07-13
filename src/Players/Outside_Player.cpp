#include "Players/Outside_Player.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"


// Only implement CECP for now and see how much can be generalized.

Outside_Player::Outside_Player() : log_file_name("chess_log.txt")
{
    log("==================");
    std::string data_in;
    data_in = receive_command();
    if(data_in == "xboard")
    {
        log("as expected");
    }
    else
    {
        log("?????");
    }
    data_in = receive_command();
    if(data_in == "protover 2")
    {
        log("as expected, setting options");
        send_command("feature usermove=1 sigint=0 sigterm=0 reuse=0 done=1");
    }
    else
    {
        log("?????????");
    }
}

Outside_Player::~Outside_Player()
{
}

Color Outside_Player::get_ai_color() const
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
std::string Outside_Player::name() const
{
    return "CECP Interface Player";
}

const Complete_Move Outside_Player::choose_move(const Board& board, const Clock& /*clock*/) const
{
    if(first_move.empty())
    {
        send_command("move " + board.last_move());
        return board.get_complete_move(receive_move());
    }
    else
    {
        auto move = board.get_complete_move(first_move);
        first_move.clear();
        return move;
    }
}

std::string Outside_Player::receive_command() const
{
    std::string result;
    getline(std::cin, result);
    log("RECEIVING: " + result);
    if(result == "quit")
    {
        throw std::runtime_error("Told to quit by XBoard");
    }

    return result;
}

std::string Outside_Player::receive_move() const
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

void Outside_Player::log(const std::string& data) const
{
    std::ofstream(log_file_name, std::ios::app) << data << std::endl;
}

void Outside_Player::send_command(const std::string& cmd) const
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}

void Outside_Player::process_game_ending(const Game_Ending_Exception& gee) const
{
    std::ostringstream result;
    if(gee.winner() == WHITE)
    {
        result << "1-0";
    }
    else if(gee.winner() == BLACK)
    {
        result << "0-1";
    }
    else
    {
        result << "1/2-1/2";
    }

    result << " {" << gee.what() << "}";

    send_command(result.str());
}
