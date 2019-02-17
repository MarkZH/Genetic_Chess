#ifndef Clock_H
#define Clock_H

#include <array>
#include <chrono>

#include "Color.h"

class Game_Result;
class CECP_Mediator;

class Clock
{
    public:
        // Game clock settings: total time, moves to reset clock, time increment per move
        Clock(double duration_seconds = 0.0,
              size_t moves = 0,
              double increment_seconds = 0.0,
              bool clock_stops_game = true);

        Game_Result punch(); // start/stop both clocks
        void stop(); // stop both clocks
        void start(); // resume after stop()

        double time_left(Color color) const;
        size_t moves_to_reset(Color color) const;
        Color running_for() const;
        double running_time_left() const;

        std::chrono::system_clock::time_point game_start_date_and_time() const;
        double initial_time() const;
        size_t moves_to_reset() const;
        double increment() const;

    private:
        using fractional_seconds = std::chrono::duration<double>;

        // timers are mutable so they can be adjusted by external interfaces (xboard, UCI, etc.)
        mutable std::array<fractional_seconds, 2> timers;
        std::array<size_t, 2> moves_to_reset_clocks;

        fractional_seconds initial_start_time;
        fractional_seconds increment_time;
        size_t move_count_reset;

        Color whose_turn;
        bool use_clock;
        bool use_reset;
        bool clocks_running;
        bool local_clock_stoppage;
        std::chrono::system_clock::time_point game_start_date_time;

        // mutable for external adjustment
        mutable std::chrono::steady_clock::time_point time_previous_punch;

        // When playing with outside interfaces, use the external clock
        friend class CECP_Mediator;
        void set_time(Color player, double new_time_seconds) const;
};

#endif // Clock_H
