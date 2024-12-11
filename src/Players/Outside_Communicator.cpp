#include "Players/Outside_Communicator.h"

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <future>
#include <format>

#include "Game/Color.h"
#include "Players/Proxy_Player.h"

#include "Players/Xboard_Mediator.h"
#include "Players/UCI_Mediator.h"

#include "Utility/Exceptions.h"

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player, const bool enable_logging)
{
    std::string protocol_type;
    std::getline(std::cin, protocol_type);
    auto mediator = std::unique_ptr<Outside_Communicator>();
    if(protocol_type == "xboard")
    {
        mediator = std::make_unique<Xboard_Mediator>(player, enable_logging);
    }
    else if(protocol_type == "uci")
    {
        mediator = std::make_unique<UCI_Mediator>(player, enable_logging);
    }
    else
    {
        throw std::runtime_error(std::format("Unrecognized protocol: {}", protocol_type));
    }

    mediator->log(protocol_type);
    return mediator;
}

Outside_Communicator::Outside_Communicator(const bool enable_logging)
{
    if(enable_logging)
    {
        const auto log_time_stamp = String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d-%H.%M.%S");
        const auto log_file_name = std::format("chess-comm-log-{}-{}.txt", log_time_stamp, Random::random_string(8));
        ofs = std::ofstream(log_file_name);
        if( ! ofs)
        {
            throw std::runtime_error(std::format("Could not write to file: {}", log_file_name));
        }
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
        remote_opponent_name = std::format("{} {}", title, name);
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

        log("RECEIVING: {}", result);
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

void Outside_Communicator::listen(Clock& clock)
{
    last_listening_result = std::async(std::launch::async, &Outside_Communicator::listener, this, std::ref(clock));
}
