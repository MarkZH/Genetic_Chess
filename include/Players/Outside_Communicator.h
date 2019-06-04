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
        virtual ~Outside_Communicator();

        virtual void setup_turn(Board& board, Clock& clock) = 0;
        virtual void listen(Board& board, Clock& clock) = 0;
        virtual void handle_move(Board& board, const Move& move) = 0;
        virtual bool pondering_allowed() const= 0;

        static void log(const std::string& data);

    protected:
        static std::string indent;

        Outside_Communicator();

        static void send_command(const std::string& cmd);
        static std::string receive_command();

        friend std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);
};

std::unique_ptr<Outside_Communicator> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
