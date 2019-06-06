#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>

class Clock;
class Board;
class Move;
class Player;

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
        virtual void handle_move(Board& board, const Move& move) = 0;

        //! Returns whether the GUI is allowing thinking during the other player's move.
        virtual bool pondering_allowed() const= 0;

        static void log(const std::string& data);

    protected:
        //! If two copies of genetic_chess are running, their logs can be indented differently to distinguish them.
        static std::string indent;

        //! Constructor is protected so that it is only called by connect_to_outside().
        Outside_Communicator() = default;

        static void send_command(const std::string& cmd);
        static std::string receive_command();

        friend std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);
};

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
