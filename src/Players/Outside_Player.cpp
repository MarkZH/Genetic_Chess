#include "Players/Outside_Player.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "Moves/Complete_Move.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Players/Player.h"
#include "Players/CECP_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Utility.h"

std::string Outside_Player::log_file_name = "chess_comm_log.txt";
std::string Outside_Player::indent = "";

std::unique_ptr<Outside_Player> connect_to_outside(const Player& player)
{
    Outside_Player::log("==================");
    auto protocol_type = Outside_Player::receive_command();
    if(protocol_type == "xboard")
    {
        return std::make_unique<CECP_Mediator>(player);
    }
    else if(protocol_type == "uci")
    {
        return std::make_unique<UCI_Mediator>(player);
    }
    else
    {
        throw std::runtime_error("Unrecognized protocol: " + protocol_type);
    }
}

Outside_Player::Outside_Player() :
    starting_game_time(0),
    moves_to_reset_clock(0),
    time_increment(0),
    got_clock(false)
{
}

std::string Outside_Player::receive_command()
{
    std::string result;
    getline(std::cin, result);
    log("RECEIVING: " + result);
    if(result == "quit")
    {
        throw std::runtime_error("Quit.");
    }

    return result;
}

void Outside_Player::log(const std::string& data)
{
    std::ofstream(log_file_name, std::ios::app) << indent << data << std::endl;
}

void Outside_Player::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}

int Outside_Player::get_game_time()
{
    if( ! got_clock)
    {
        get_clock_specs();
        got_clock = true;
    }
    return starting_game_time;
}

int Outside_Player::get_reset_moves()
{
    if( ! got_clock)
    {
        get_clock_specs();
        got_clock = true;
    }
    return moves_to_reset_clock;
}

int Outside_Player::get_increment()
{
    if( ! got_clock)
    {
        get_clock_specs();
        got_clock = true;
    }
    return time_increment;
}

void Outside_Player::set_game_time(int time)
{
    starting_game_time = time;
}

void Outside_Player::set_reset_moves(int moves)
{
    moves_to_reset_clock = moves;
}

void Outside_Player::set_increment(int increment)
{
    time_increment = increment;
}
