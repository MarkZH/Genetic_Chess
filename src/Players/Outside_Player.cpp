#include "Players/Outside_Player.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <iomanip>

#include "Players/CECP_Mediator.h"
#include "Exceptions/Game_Ended.h"
#include "Utility/Scoped_Stopwatch.h"
#include "Utility/String.h"

std::string Outside_Player::indent = "\t";

//! Initialize communication with an outside program.
//
//! The function returns an appropriate derived class based on the
//! communiation protocol by the other program. CECP is the only
//! protocol implemented so far. UCI may be implemented in the future.
//! \param player The local player so that its name may be sent to the outside interface.
//! \returns An appropriate derived class instance with the proper protocol.
std::unique_ptr<Outside_Player> connect_to_outside(const Player& player)
{
    Outside_Player::log("==================");
    auto protocol_type = Outside_Player::receive_command();
    if(protocol_type == "xboard")
    {
        return std::make_unique<CECP_Mediator>(player);
    }
    else
    {
        throw std::runtime_error("Unrecognized protocol: " + protocol_type);
    }
}

//! Initialize a generic outside interface with no clock data.
Outside_Player::Outside_Player() :
    starting_game_time(0),
    moves_to_reset_clock(0),
    time_increment(0),
    got_clock(false)
{
}

Outside_Player::~Outside_Player()
{
    log("Shutting down.");
}

//! Wait for a command from the outside interface and pass it on to derived class instances.
//
//! \returns The command from the outside interface if not "quit".
//! \throws Game_Ended If the command "quit" is received, the game will end and the program will exit.
std::string Outside_Player::receive_command()
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
void Outside_Player::log(const std::string& data)
{
    static const Scoped_Stopwatch running_time("");
    static const auto log_file_name = "chess_comm_log.txt";
    std::ofstream(log_file_name, std::ios::app) << std::setw(12) << running_time.time_so_far() << indent << data << std::endl;
}

//! Output the given string to the outside interface.
//
//! The outgoing string is also logged to a local file.
//! \param cmd The string to send to the outside interface.
void Outside_Player::send_command(const std::string& cmd)
{
    log("SENDING: " + cmd);
    std::cout << cmd << std::endl;
}

//! Reports the initial game time received from the outside interface.
//
//! \returns The initial time in seconds to be put on the clock.
double Outside_Player::game_time()
{
    if( ! got_clock)
    {
        receive_clock_specs();
        got_clock = true;
    }
    return starting_game_time;
}

//! Reports the number of moves that will reset the game clocks according to the outside interface.
//
//! \returns The number of moves to reset game clocks for setting up local clocks.
size_t Outside_Player::reset_moves()
{
    if( ! got_clock)
    {
        receive_clock_specs();
        got_clock = true;
    }
    return moves_to_reset_clock;
}

//! The amount of time to add to the game clocks after each move according to the outside interface.
//
//! \returns The amount of time in seconds to add to local clocks after each move.
double Outside_Player::increment()
{
    if( ! got_clock)
    {
        receive_clock_specs();
        got_clock = true;
    }
    return time_increment;
}

//! Record the the initial game time as received from outside interfaces.
//
//! \param time Time in seconds.
void Outside_Player::set_game_time(double time)
{
    starting_game_time = time;
}

//! Record the number of moves to reset game clocks as received from outside interfaces.
//
//! \param moves Number of moves.
void Outside_Player::set_reset_moves(size_t moves)
{
    moves_to_reset_clock = moves;
}

//! Record the time to add to clocks after each moves as received from outside interfaces.
//
//! \param increment Time in seconds.
void Outside_Player::set_increment(double increment)
{
    time_increment = increment;
}

//! When interfacing with outside programs, that program keeps the official time.
//
//! \returns False to prevent the in-program clock from stopping the game.
bool Outside_Player::stop_for_local_clock() const
{
    return false;
}

bool Outside_Player::print_game_to_stdout() const
{
    return false;
}
