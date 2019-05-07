#include "Game/Clock.h"

#include <array>
#include <chrono>
using namespace std::chrono_literals;
#include <cassert>
#include <limits>

#include "Game/Game_Result.h"

//! Constructs a game clock for timing games.
//
//! \param duration_seconds The initial amount of time on the clock.
//!        If this is zero, then the clock is inactive and will not stop the game.
//! \param moves_to_reset The number of moves before the clocks are reset to the initial time.
//! \param increment_seconds Amount of time to add to a player's clock after every move.
//! \param starting_turn Which player's clock to start upon calling start().
//! \param clock_stops_game Whether this clock should return a Game_Result that stops a game upon time expiring.
//!        This parameter should be false if an external clock (e.g., from XBoard via CECP) will stop the game.
Clock::Clock(double duration_seconds,
             size_t moves_to_reset,
             double increment_seconds,
             Color starting_turn,
             bool clock_stops_game) :
    timers({fractional_seconds(duration_seconds), fractional_seconds(duration_seconds)}),
    moves_to_reset_clocks({0, 0}),
    initial_start_time(duration_seconds),
    increment_time(increment_seconds),
    move_count_reset(moves_to_reset),
    whose_turn(starting_turn),
    use_clock(duration_seconds > 0),
    use_reset(moves_to_reset > 0),
    clocks_running(false),
    local_clock_stoppage(clock_stops_game)
{
}

//! Stop the current player's clock and restart the opponent's clock.
Game_Result Clock::punch()
{
    assert(clocks_running);

    auto time_this_punch = std::chrono::steady_clock::now();

    if( ! use_clock)
    {
        return {};
    }

    timers[whose_turn] -= (time_this_punch - time_previous_punch);
    if(local_clock_stoppage && timers[whose_turn] < 0s)
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

//! Stop both clocks.
void Clock::stop()
{
    auto time_stop = std::chrono::steady_clock::now();
    timers[whose_turn] -= (time_stop - time_previous_punch);
    clocks_running = false;
}

//! Start the moving player's clock at the start of a game.
//
//! This method also records the start time of the game for use in Board::print_game_record().
void Clock::start()
{
    time_previous_punch = std::chrono::steady_clock::now();
    game_start_date_time = std::chrono::system_clock::now();
    clocks_running = true;
}

//! Returns the amount of time left for the given player.
//
//! \param color The color of the player whose time is being queried.
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

//! The number of moves left before the clocks reset to the initial time.
//
//! \param color The color of the player being queried.
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

//! The player for whom the clock is running.
Color Clock::running_for() const
{
    return whose_turn;
}

//! Adjust the time on the clocks.
//
//! \param player Indicates which clock to adjust.
//! \param new_time_seconds The new amount of time left on that clock.
//! This method is used by the Players derived from Outside_Player to adjust
//! the clock according to instructions from chess GUIs using the CECP protocol.
void Clock::set_time(Color player, double new_time_seconds) const
{
    timers[player] = fractional_seconds(new_time_seconds);
    time_previous_punch = std::chrono::steady_clock::now();
}

//! The amount of time left on the clock that is currently running.
double Clock::running_time_left() const
{
    return time_left(running_for());
}

//! Returns the date and time when start() was called.
std::chrono::system_clock::time_point Clock::game_start_date_and_time() const
{
    return game_start_date_time;
}

//! How many moves must be played before the clocks are reset to their initial times.
size_t Clock::moves_to_reset() const
{
    return move_count_reset;
}

//! The intitial time on the clocks at the start of the game (and after moves_to_reset()).
double Clock::initial_time() const
{
    return initial_start_time.count();
}

//! How much time is added to a player's clock after every move.
double Clock::increment() const
{
    return increment_time.count();
}
