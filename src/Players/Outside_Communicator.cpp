#include "Players/Outside_Communicator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <stdexcept>

#include "Game/Color.h"
#include "Players/CECP_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"
#include "Utility/Random.h"

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player)
{
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

    while(result.empty())
    {
        if( ! std::getline(std::cin, result))
        {
            log("GUI disconnected");
            throw Game_Ended("GUI disconnected");
        }

        log("RECEIVING: " + result);
        result = String::remove_extra_whitespace(result);
    }

    if(result == "quit")
    {
        throw Game_Ended(result);
    }
    else
    {
        return result;
    }
}

void Outside_Communicator::log(const std::string& data)
{
    static const auto log_time_stamp = String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d-%H.%M.%S");
    static const auto log_file_name = "chess-comm-log-" + log_time_stamp + "-" + Random::random_string(8) + ".txt";
    static auto ofs = std::ofstream(log_file_name);
    if( ! ofs)
    {
        throw std::runtime_error("Could not write to file: " + log_file_name);
    }

    auto item_time_stamp = String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d %H:%M:%S");
    ofs << item_time_stamp
        << " -- "
        << data
        << std::endl;
}

void Outside_Communicator::send_command(const std::string& cmd) const noexcept
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}
