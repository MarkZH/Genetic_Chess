#include "Game/Clock.h"

#include <array>
#include <chrono>
#include <cassert>
#include <limits>

#include "Game/Game_Result.h"

Clock::Clock(double duration_seconds,
             size_t moves_to_reset,
             double increment_seconds,
             bool clock_stops_game) :
    timers({fractional_seconds(duration_seconds), fractional_seconds(duration_seconds)}),
    moves_to_reset_clocks({0, 0}),
    initial_start_time(duration_seconds),
    increment_time(increment_seconds),
    move_count_reset(moves_to_reset),
    whose_turn(WHITE),
    use_clock(duration_seconds > 0),
    use_reset(moves_to_reset > 0),
    clocks_running(false),
    local_clock_stoppage(clock_stops_game)
{
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
    if(local_clock_stoppage && timers[whose_turn] < std::chrono::seconds(0))
    {
        stop();
        return Game_Result(opposite(whose_turn), "Time Forfeiture");
    }

    if(use_reset && (++moves_to_reset_clocks[whose_turn] == move_count_reset))
    {
        timers[whose_turn] += initial_start_time;
        moves_to_reset_clocks[whose_turn] = 0;
    }

    whose_turn = opposite(whose_turn);
    time_previous_punch = time_this_punch;
    timers[whose_turn] += increment_time;

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
    game_start_date_time = std::chrono::system_clock::now();
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
        return timers[color].count();
    }
    else
    {
        auto now = std::chrono::steady_clock::now();
        return fractional_seconds(timers[color] - (now - time_previous_punch)).count();
    }
}

size_t Clock::moves_to_reset(Color color) const
{
    if(use_reset)
    {
        return move_count_reset - moves_to_reset_clocks[color];
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
    time_previous_punch = std::chrono::steady_clock::now();
}

double Clock::running_time_left() const
{
    return time_left(running_for());
}

std::chrono::system_clock::time_point Clock::game_start_date_and_time() const
{
    return game_start_date_time;
}

size_t Clock::moves_to_reset() const
{
    return move_count_reset;
}

double Clock::initial_time() const
{
    return initial_start_time.count();
}

double Clock::increment() const
{
    return increment_time.count();
}
