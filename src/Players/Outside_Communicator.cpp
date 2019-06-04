#include "Players/Outside_Communicator.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <chrono>

#include "Players/CECP_Mediator.h"
#include "Exceptions/Game_Ended.h"

#include "Utility/String.h"
#include "Utility/Random.h"

std::string Outside_Communicator::indent = "\t";

//! Initialize communication with an outside program.
//
//! The function returns an appropriate derived class based on the
//! communiation protocol by the other program. CECP is the only
//! protocol implemented so far. UCI may be implemented in the future.
//! \param player The local player so that its information may be sent
//!        to the outside interface.
//! \returns An appropriate derived class instance with the proper protocol.
std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player)
{
    Outside_Communicator::log("==================");
    auto protocol_type = Outside_Communicator::receive_command();
    if(protocol_type == "xboard")
    {
        return std::make_unique<CECP_Mediator>(player);
    }
    else
    {
        throw std::runtime_error("Unrecognized protocol: " + protocol_type);
    }
}

//! Initialize a generic outside interface.
Outside_Communicator::Outside_Communicator()
{
}

Outside_Communicator::~Outside_Communicator()
{
    log("Shutting down.");
}

//! Wait for a command from the outside interface and pass it on to derived class instances.
//
//! \returns The command from the outside interface if not "quit".
//! \throws Game_Ended If the command "quit" is received, the game will end and the program will exit.
std::string Outside_Communicator::receive_command()
{
    std::string result;
    std::getline(std::cin, result);
    log("RECEIVING: " + result);
    if(String::remove_extra_whitespace(result).empty())
    {
        log("Got empty input from GUI. Shutting down.");
        result = "quit";
    }

    if(result == "quit")
    {
        throw Game_Ended(NONE, "Told to quit");
    }

    return result;
}

//! Log data to a local text file.
//
//! \param data A text string to write.
void Outside_Communicator::log(const std::string& data)
{
    static const auto log_file_name = "chess_comm_log.txt";
    std::ofstream(log_file_name, std::ios::app)
        << String::date_and_time_format(std::chrono::system_clock::now(), "%Y.%m.%d %H:%M:%S --")
        << indent
        << data
        << std::endl;
}

//! Output the given string to the outside interface.
//
//! The outgoing string is also logged to a local file.
//! \param cmd The string to send to the outside interface.
void Outside_Communicator::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}