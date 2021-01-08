#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>
#include <vector>

#include "Game/Color.h"

class Clock;
class Board;
class Move;
class Player;
class Game_Result;

//! \brief A class to facilitate interfacing with outside programs.
class Outside_Communicator
{
    public:
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
        //! \param board The Board used for the game.
        //! \param clock The clock used for the game.
        virtual void listen(const Board& board, Clock& clock) = 0;

        //! \brief When appropriate, apply the local AIs Move to Board and send results to GUI.
        //!
        //! \param board The Board used for the game.
        //! \param move The move picked by the local AI.
        //! \param move_list The list of moves in the game so far.
        virtual Game_Result handle_move(Board& board,
                                        const Move& move,
                                        std::vector<const Move*>& move_list) const = 0;

        //! \brief Log data to a local text file.
        //!
        //! \param data A text string to write.
        static void log(const std::string& data);

    protected:
        //! \brief Constructor is protected so that it is only called by connect_to_outside().
        Outside_Communicator() = default;

        //! \brief Output the given string to the outside interface.
        //!
        //! The outgoing string is also logged to a local file.
        //! \param cmd The string to send to the outside interface.
        void send_command(const std::string& cmd) const noexcept;

        //! \brief Wait for a command from the outside interface and pass it on to derived class instances.
        //!
        //! \returns The command from the outside interface if not "quit".
        //! \exception Game_Ended If the command "quit" is received or an error occurs, the game will
        //!         end and the program will exit.
        static std::string receive_command();

        friend std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);
};

//! \brief Initialize communication with an outside program.
//!
//! The function returns an appropriate derived class based on the
//! communiation protocol by the other program (CECP or UCI).
//! \param player The local player so that its information may be sent
//!        to the outside interface.
//! \returns An appropriate derived class instance with the proper protocol.
std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
