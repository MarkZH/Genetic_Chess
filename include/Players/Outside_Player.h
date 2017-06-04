#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>

#include "Player.h"

#include "Game/Color.h"

class Clock;

// For interfacing with outside programs
class Outside_Player : public Player
{
    public:
        ~Outside_Player() override;

        virtual Color get_ai_color() const = 0;

        int get_game_time();
        int get_reset_moves();
        int get_increment();

    protected:
        Outside_Player();

        static std::string indent;

        static void send_command(const std::string& cmd);
        static std::string receive_command();

        virtual std::string receive_move(const Clock& clock) const = 0;

        static void log(const std::string& data);

        void set_game_time(int time);
        void set_reset_moves(int moves);
        void set_increment(int increment);

    private:
        static std::string log_file_name;

        int starting_game_time;
        int moves_to_reset_clock;
        int time_increment;
        bool got_clock;

        virtual void get_clock_specs() = 0;

        friend std::unique_ptr<Outside_Player> connect_to_outside(const Player& player);
};

std::unique_ptr<Outside_Player> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
