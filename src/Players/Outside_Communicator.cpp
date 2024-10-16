#include "Players/Outside_Communicator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <stdexcept>
#include <future>

#include "Game/Color.h"
#include "Players/Proxy_Player.h"

#include "Players/Xboard_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"
#include "Utility/Random.h"

namespace
{
    auto log_communication = true;
}

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player, const bool enable_logging)
{
    log_communication = enable_logging;
    const auto protocol_type = Outside_Communicator::receive_command();
    if(protocol_type == "xboard")
    {
        return std::make_unique<Xboard_Mediator>(player);
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

void Outside_Communicator::record_opponent_name(const std::string& name, const std::string& title) noexcept
{
    if(title == "none" || title.empty())
    {
        remote_opponent_name = name;
    }
    else
    {
        remote_opponent_name = title + " " + name;
    }
}

Proxy_Player Outside_Communicator::create_proxy_player() const noexcept
{
    return Proxy_Player{remote_opponent_name};
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

std::string Outside_Communicator::get_last_command(const bool while_listening)
{
    if(while_listening)
    {
        return receive_command();
    }
    else
    {
        return last_listening_result.valid() ? last_listening_result.get() : receive_command();
    }
}

void Outside_Communicator::log(const std::string& data)
{
    if( ! log_communication)
    {
        return;
    }

    static const auto log_time_stamp = String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d-%H.%M.%S");
    static const auto log_file_name = "chess-comm-log-" + log_time_stamp + "-" + Random::random_string(8) + ".txt";
    static auto ofs = std::ofstream(log_file_name);
    if( ! ofs)
    {
        throw std::runtime_error("Could not write to file: " + log_file_name);
    }

    ofs << String::date_and_time_format<std::chrono::milliseconds>(std::chrono::system_clock::now(), "%Y.%m.%d %H:%M:%S")
        << " -- "
        << data
        << std::endl;
}

void Outside_Communicator::send_command(const std::string& cmd) noexcept
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}

void Outside_Communicator::listen(Clock& clock)
{
    last_listening_result = std::async(std::launch::async, &Outside_Communicator::listener, this, std::ref(clock));
}
