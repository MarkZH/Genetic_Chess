#include "Players/Outside_Player.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Players/CECP_Mediator.h"

#include "Exceptions/Game_Ending_Exception.h"

#include "Utility.h"

std::string Outside_Player::log_file_name = "chess_comm_log.txt";

std::unique_ptr<Outside_Player> connect_to_outside()
{
    Outside_Player::log("==================");
    if(Outside_Player::receive_command() == "xboard")
    {
        return std::unique_ptr<Outside_Player>(new CECP_Mediator);
    }
    else
    {
        throw std::runtime_error("Only CECP protocol implemented.");
    }
}

Outside_Player::~Outside_Player()
{
}

std::string Outside_Player::receive_command()
{
    std::string result;
    getline(std::cin, result);
    log("RECEIVING: " + result);
    if(result == "quit")
    {
        throw std::runtime_error("Told to quit.");
    }

    return result;
}

void Outside_Player::log(const std::string& data)
{
    std::ofstream(log_file_name, std::ios::app) << data << std::endl;
}

void Outside_Player::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}
