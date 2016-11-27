#ifndef Clock_H
#define Clock_H

#include <map>
#include <chrono>

#include "Color.h"

class Clock
{
    public:
        Clock(int seconds = 0, size_t moves = 0); // number of seconds per number of moves
        void punch(); // start/stop both clocks
        void stop(); // stop both clocks
        void start(); // resume after stop()
        double time_left(Color color) const;
        bool is_running() const;
        Color running_for() const;

    private:
        std::map<Color, std::chrono::steady_clock::duration> timers; // in clock ticks
        std::map<Color, int> moves; // move count
        std::map<Color, std::chrono::steady_clock::duration> initial_time; // for resetting after moves
        Color whose_turn; // black pushes clock to start game
        bool use_clock; // = false for default constructor
        bool use_reset; // = false for simple timed game, true if, e.g., game is 40 moves per hour
        int move_count_reset;
        bool clocks_running;
        std::chrono::steady_clock::time_point time_previous_punch;
};

#endif // Clock_H
