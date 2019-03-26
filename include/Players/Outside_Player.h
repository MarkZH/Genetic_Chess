#ifndef OUTSIDE_PLAYER_H
#define OUTSIDE_PLAYER_H

#include <string>
#include <memory>

#include "Player.h"

#include "Game/Color.h"
#include "Utility/Scoped_Stopwatch.h"

class Clock;

//! A class to facilitate interfacing with outside programs.
class Outside_Player : public Player
{
    public:
        //! Returns the color to be controlled by the local AI.
        virtual Color ai_color() const = 0;

        double game_time();
        size_t reset_moves();
        double increment();

        bool stop_for_local_clock() const override;

    protected:
        Outside_Player();

        //! Once the game starts, the logged data from different players is indented differently to help distinguish between them.
        static std::string indent;

        static void send_command(const std::string& cmd);
        static std::string receive_command();

        //! Wait for a string specifying the move made by an outside player.

        //! \param clock The local game clock is passed in so that it can be adjusted
        //!        to better match the interaface's clock.
        //! \returns A text string with the move to be interpretted by Board::create_move();
        virtual std::string receive_move(const Clock& clock) const = 0;

        static void log(const std::string& data);

        void set_game_time(double time);
        void set_reset_moves(size_t moves);
        void set_increment(double increment);

    private:
        static std::string log_file_name;
        static Scoped_Stopwatch running_time;

        double starting_game_time;
        size_t moves_to_reset_clock;
        double time_increment;
        bool got_clock;

        virtual void receive_clock_specs() = 0;

        friend std::unique_ptr<Outside_Player> connect_to_outside(const Player& player);
};

std::unique_ptr<Outside_Player> connect_to_outside(const Player& player);

#endif // OUTSIDE_PLAYER_H
