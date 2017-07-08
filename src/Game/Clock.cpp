#include "Game/Clock.h"

#include <map>
#include <chrono>
#include <cassert>

#include "Game/Game_Result.h"

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

    moves[WHITE] = 0;
    moves[BLACK] = 0;
}

bool Clock::is_running() const
{
    return clocks_running;
}

Game_Result Clock::punch()
{
    assert(clocks_running);

    auto time_this_punch = std::chrono::steady_clock::now();

    if( ! use_clock)
    {
        return {};
    }

    timers[whose_turn] -= (time_this_punch - time_previous_punch);
    if(timers[whose_turn] < std::chrono::seconds(0))
    {
        stop();
        return Game_Result(opposite(whose_turn), "Time Forfeiture", true);
    }

    if(use_reset && (++moves[whose_turn] == move_count_reset))
    {
        timers[whose_turn] += initial_time[whose_turn];
        moves[whose_turn] = 0;
    }

    whose_turn = opposite(whose_turn);
    time_previous_punch = time_this_punch;
    timers[whose_turn] += increment[whose_turn];

    return {};
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
        return timers.at(color).count();
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
        return move_count_reset - moves.at(color);
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

void Clock::set_time(Color player, double new_time_seconds) const
{
    timers[player] = fractional_seconds(new_time_seconds);
}
