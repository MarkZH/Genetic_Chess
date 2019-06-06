#ifndef Clock_H
#define Clock_H

#include <array>
#include <chrono>

#include "Color.h"

class Game_Result;
class CECP_Mediator;
class UCI_Mediator;

//! This class represents the dual-clock game timers used in tournament chess.
class Clock
{
    public:
        Clock(double duration_seconds = 0.0,
              size_t moves_to_reset = 0,
              double increment_seconds = 0.0,
              Color starting_turn = WHITE,
              bool clock_stops_game = true);

        Game_Result punch();
        void stop();
        void start();
        void do_not_stop_game();

        double time_left(Color color) const;
        size_t moves_until_reset(Color color) const;
        Color running_for() const;
        double running_time_left() const;
        bool is_running() const;

        std::chrono::system_clock::time_point game_start_date_and_time() const;
        double initial_time() const;
        double increment(Color color) const;
        size_t moves_per_time_period() const;

    private:
        using fractional_seconds = std::chrono::duration<double>;

        std::array<fractional_seconds, 2> timers;
        std::array<size_t, 2> moves_to_reset_clocks;

        fractional_seconds initial_start_time;
        std::array<fractional_seconds, 2> increment_time;
        size_t move_count_reset;

        Color whose_turn;
        bool use_clock;
        bool clocks_running;
        bool local_clock_stoppage;

        std::chrono::system_clock::time_point game_start_date_time;
        std::chrono::steady_clock::time_point time_previous_punch;

        friend class CECP_Mediator;
        friend class UCI_Mediator;

        void set_time(Color player, double new_time_seconds);
        void set_increment(Color player, double new_increment_time_seconds);
        void set_next_time_reset(size_t moves_to_reset);
};

#endif // Clock_H
