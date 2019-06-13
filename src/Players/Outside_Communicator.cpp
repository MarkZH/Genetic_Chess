#include "Players/Outside_Communicator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>

#include "Players/CECP_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"

std::string Outside_Communicator::indent = "\t";

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player)
{
    Outside_Communicator::log("==================");
    auto protocol_type = Outside_Communicator::receive_command();
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

Outside_Communicator::~Outside_Communicator()
{
    log("Shutting down.");
}

std::string Outside_Communicator::receive_command()
{
    std::string result;
    std::getline(std::cin, result);
    log("RECEIVING: " + result);
    result = String::remove_extra_whitespace(result);

    if(result.empty())
    {
        log("Error in communication. Got empty message from GUI.");
        throw Game_Ended();
    }

    if(result == "quit")
    {
        throw Game_Ended();
    }

    return result;
}

void Outside_Communicator::log(const std::string& data)
{
    static const auto log_file_name = "chess_comm_log.txt";
    std::ofstream(log_file_name, std::ios::app)
        << String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d %H:%M:%S --")
        << indent
        << data
        << std::endl;
}

void Outside_Communicator::set_indent_level(unsigned int n)
{
    indent = std::string(n, '\t');
}

void Outside_Communicator::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}
