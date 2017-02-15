#include "Game/Clock.h"

#include <map>
#include <chrono>

#include "Exceptions/Out_Of_Time_Exception.h"

Clock::Clock(double duration_seconds, size_t moves_to_reset, double increment_seconds) :
    whose_turn(WHITE),
    use_clock(duration_seconds > 0),
    use_reset(moves_to_reset > 0),
    move_count_reset(moves_to_reset),
    clocks_running(false)
{
    timers[WHITE] = fractional_seconds(duration_seconds);
    timers[BLACK] = fractional_seconds(duration_seconds);

    initial_time[WHITE] = fractional_seconds(duration_seconds);
    initial_time[BLACK] = fractional_seconds(duration_seconds);

    increment[WHITE] = fractional_seconds(increment_seconds);
    increment[BLACK] = fractional_seconds(increment_seconds);
}

bool Clock::is_running() const
{
    return clocks_running;
}

void Clock::punch()
{
    auto time_this_punch = std::chrono::steady_clock::now();

    if( ! use_clock)
    {
        return;
    }

    if( ! clocks_running)
    {
        throw std::runtime_error("Clock has not been started.");
    }

    timers[whose_turn] -= (time_this_punch - time_previous_punch);
    if(timers[whose_turn] < std::chrono::seconds(0))
    {
        throw Out_Of_Time_Exception(whose_turn);
    }

    if(use_reset && (++moves[whose_turn] == move_count_reset))
    {
        timers[whose_turn] = initial_time[whose_turn];
        moves[whose_turn] = 0;
    }

    whose_turn = opposite(whose_turn);
    time_previous_punch = time_this_punch;
    timers[whose_turn] += increment[whose_turn];
}

void Clock::stop()
{
    auto time_stop = std::chrono::steady_clock::now();
    timers[whose_turn] -= (time_stop - time_previous_punch);
    clocks_running = false;
}

void Clock::start()
{
    time_previous_punch = std::chrono::steady_clock::now();
    clocks_running = true;
}


double Clock::time_left(Color color) const
{
    if( ! use_clock)
    {
        return 0.0;
    }
    if(whose_turn != color || ! clocks_running)
    {
        return fractional_seconds(timers.at(color)).count();
    }
    else
    {
        auto now = std::chrono::steady_clock::now();
        return fractional_seconds(timers.at(color) - (now - time_previous_punch)).count();
    }
}

int Clock::moves_to_reset(Color color) const
{
    if(use_reset)
    {
        return move_count_reset - (moves.at(color) % move_count_reset);
    }
    else
    {
        return std::numeric_limits<int>::max();
    }
}

Color Clock::running_for() const
{
    return whose_turn;
}
