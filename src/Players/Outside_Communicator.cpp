#include "Players/Outside_Communicator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>

#include "Game/Color.h"
#include "Players/CECP_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"

namespace
{
    std::string indent;
}

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player)
{
    Outside_Communicator::set_log_indent(NONE);
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
    if( ! std::getline(std::cin, result))
    {
        log("GUI disconnected");
        throw Game_Ended();
    }

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

std::string Outside_Communicator::other_player_name() const
{
    return outside_player_name;
}

void Outside_Communicator::set_log_indent(Color color) noexcept
{
    indent = std::string((static_cast<int>(color) + 1)%3 + 1, '\t');
}

void Outside_Communicator::set_other_player_name(const std::string& name)
{
    outside_player_name = name;
}

void Outside_Communicator::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}
