#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>

class Clock;
class Board;
class Move;
class Player;
class Game_Result;

//! A class to facilitate interfacing with outside programs.
class Outside_Communicator
{
    public:
        //! Records that the communication channel is shutting down.
        virtual ~Outside_Communicator();

        //! Adjust the game board and clock according to received instructions.
        //
        //! \param board The Board used for the game.
        //! \param clock The clock used for the game.
        virtual void setup_turn(Board& board, Clock& clock) = 0;

        //! Start a separate thread to listen for commands while the local AI is thinking.
        //
        //! \param board The Board used for the game.
        //! \param clock The clock used for the game.
        virtual void listen(Board& board, Clock& clock) = 0;

        //! When appropriate, apply the local AIs Move to Board and send results to GUI.
        //
        //! \param board The Board used for the game.
        //! \param move The move picked by the local AI.
        virtual Game_Result handle_move(Board& board, const Move& move) = 0;

        //! Returns whether the GUI is allowing thinking during the other player's move.
        virtual bool pondering_allowed() const = 0;

        //! Log data to a local text file.
        //
        //! \param data A text string to write.
        static void log(const std::string& data);

        //! Get the name of the player on the other side of the GUI.
        std::string other_player_name() const;

    protected:
        //! If two copies of genetic_chess are running, their logs can be indented differently to distinguish them.
        static void set_indent_level(unsigned int n);

        //! Store the opponent's name when received from the GUI.
        //
        //! \param name The received name.
        void set_other_player_name(const std::string& name);

        //! Constructor is protected so that it is only called by connect_to_outside().
        Outside_Communicator() = default;

        //! Output the given string to the outside interface.
        //
        //! The outgoing string is also logged to a local file.
        //! \param cmd The string to send to the outside interface.
        static void send_command(const std::string& cmd);

        //! Wait for a command from the outside interface and pass it on to derived class instances.
        //
        //! \returns The command from the outside interface if not "quit".
        //! \throws Game_Ended If the command "quit" is received, the game will end and the program will exit.
        static std::string receive_command();

    private:
        static std::string indent;
        std::string outside_player_name;

        friend std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);
};

//! Initialize communication with an outside program.
//
//! The function returns an appropriate derived class based on the
//! communiation protocol by the other program. CECP is the only
//! protocol implemented so far. UCI may be implemented in the future.
//! \param player The local player so that its information may be sent
//!        to the outside interface.
//! \returns An appropriate derived class instance with the proper protocol.
std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
