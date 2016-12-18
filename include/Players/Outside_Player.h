#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>

#include "Player.h"

#include "Game/Color.h"

// For interfacing with Xboard
class Outside_Player : public Player
{
    public:
        Outside_Player();
        ~Outside_Player() override;

        const Complete_Move choose_move(const Board& b, const Clock& clock) const override;
        void process_game_ending(const Game_Ending_Exception& gee) const;

        std::string name() const;

        Color get_ai_color() const;

    private:
        std::string receive_command() const;
        std::string receive_move() const;
        void send_command(const std::string& cmd) const;
        void log(const std::string& data) const;

        std::string log_file_name;
        mutable std::string first_move;
};

#endif // OUTSIDE_PLAYER_H
