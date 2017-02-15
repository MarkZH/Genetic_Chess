#ifndef Clock_H
#define Clock_H

#include <map>
#include <chrono>

#include "Color.h"

using fractional_seconds = std::chrono::duration<double>;

class Clock
{
    public:
        // Game clock settings: total time, moves to reset clock, time increment per move
        Clock(double duration_seconds = 0.0, size_t moves = 0, double increment_seconds = 0.0);
        void punch(); // start/stop both clocks
        void stop(); // stop both clocks
        void start(); // resume after stop()
        double time_left(Color color) const;
        int moves_to_reset(Color color) const;
        bool is_running() const;
        Color running_for() const;

    private:
        std::map<Color, fractional_seconds> timers;
        std::map<Color, int> moves;
        std::map<Color, fractional_seconds> initial_time;
        std::map<Color, fractional_seconds> increment;
        Color whose_turn;
        bool use_clock;
        bool use_reset;
        int move_count_reset;
        bool clocks_running;
        std::chrono::steady_clock::time_point time_previous_punch;
};

#endif // Clock_H
