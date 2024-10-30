#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>
#include <vector>
#include <future>
#include <fstream>
#include <chrono>
#include <format>

#include "Game/Color.h"

#include "Utility/String.h"
#include "Utility/Random.h"

class Clock;
class Board;
class Move;
class Player;
class Proxy_Player;
class Game_Result;

class Outside_Communicator;

//! \brief Initialize communication with an outside program.
//!
//! The function returns an appropriate derived class based on the
//! communiation protocol by the other program (Xboard or UCI).
//! \param player The local player so that its information may be sent
//!        to the outside interface.
//! \param enable_logging Write all communication (excluding thinking) to a file.
//! \returns An appropriate derived class instance with the proper protocol.
std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player, bool enable_logging);

//! \brief A class to facilitate interfacing with outside programs.
class Outside_Communicator
{
    public:
        Outside_Communicator(bool enable_logging);

        //! \brief Records that the communication channel is shutting down.
        virtual ~Outside_Communicator();

        //! \brief Adjust the game board and clock according to received instructions.
        //!
        //! \param board The Board used for the game.
        //! \param clock The clock used for the game.
        //! \param move_list The list of moves in the game so far.
        //! \param player The local AI.
        virtual Game_Result setup_turn(Board& board,
                                       Clock& clock,
                                       std::vector<const Move*>& move_list,
                                       const Player& player) = 0;

        //! \brief Start a separate thread to listen for commands while the local AI is thinking.
        //!
        //! \param clock The clock used for the game.
        void listen(Clock& clock);

        //! \brief When appropriate, apply the local AIs Move to Board and send results to GUI.
        //!
        //! \param board The Board used for the game.
        //! \param move The move picked by the local AI.
        //! \param move_list The list of moves in the game so far.
        virtual Game_Result handle_move(Board& board,
                                        const Move& move,
                                        std::vector<const Move*>& move_list) const = 0;

        //! \brief Create a player for the purposes of calling Board::print_game_record()
        Proxy_Player create_proxy_player() const noexcept;

        //! \brief Log data to a local text file.
        //!
        //! \param data A text string to write.
        template<typename... Format_Args>
        void log(const std::string& data, Format_Args... args) const
        {
            if( ! ofs)
            {
                return;
            }

            flush_log_queue();
            
            std::string message;
            if constexpr (sizeof...(args) == 0)
            {
                message = data;
            }
            else
            {
                message = std::vformat(data, std::make_format_args(args...));
            }

            const auto timestamp = String::date_and_time_format<std::chrono::milliseconds>(std::chrono::system_clock::now(), "%Y.%m.%d %H:%M:%S");
            std::println(ofs, "{} -- {}", timestamp, message);
            ofs.flush();
        }

        void flush_log_queue() const;

        //! \brief Log data to a local text file.
        //!
        //! \param data A text string to write.
        template<typename... Format_Args>
        static void queue_log(const std::string& data, Format_Args... args)
        {
            if constexpr (sizeof...(args) == 0)
            {
                log_queue.push_back(data);
            }
            else
            {
                log_queue.push_back(std::vformat(data, std::make_format_args(args...)));
            }
        }

    protected:
        //! \brief Constructor is protected so that it is only called by connect_to_outside().
        Outside_Communicator() = default;

        //! \brief Record the opponent's name when told by GUI or internet mediator.
        //!
        //! \param name The received name
        //! \param title The chess title (if any) of the opponent.
        void record_opponent_name(const std::string& name, const std::string& title = {}) noexcept;

        //! \brief Output the given string to the outside interface.
        //!
        //! The outgoing string is also logged to a local file.
        //! \param cmd The string to send to the outside interface.
        static void send_command(const std::string& cmd) noexcept;

        //! \brief Wait for a command from the outside interface and pass it on to derived class instances.
        //!
        //! \returns The command from the outside interface if not "quit".
        //! \exception Game_Ended If the command "quit" is received or an error occurs, the game will
        //!         end and the program will exit.
        static std::string receive_command();

        //! \brief Returns either the command received while listening or waits for a new command.
        //!
        //! \param while_listening Indicates this method is called from within the listen() method.
        std::string get_last_command(bool while_listening);

        friend std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player, bool enable_logging);

    private:
        mutable std::ofstream ofs;
        static std::vector<std::string> log_queue;
        static bool flushing;
        std::string remote_opponent_name;
        std::future<std::string> last_listening_result;

        virtual std::string listener(Clock& clock) = 0;
};

#endif // OUTSIDE_PLAYER_H
