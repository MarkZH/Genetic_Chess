#include "Game/Clock.h"

#include <array>
#include <chrono>
using namespace std::chrono_literals;
#include <cassert>
#include <limits>
#include <sstream>

#include "Game/Board.h"
#include "Game/Game_Result.h"

Clock::Clock(seconds duration_seconds,
             size_t moves_to_reset,
             seconds increment_seconds,
             Time_Reset_Method reset_method,
             Piece_Color starting_turn,
             std::chrono::system_clock::time_point previous_start_time) noexcept :
    timers({duration_seconds, duration_seconds}),
    initial_start_time(duration_seconds),
    increment_time({increment_seconds, increment_seconds}),
    move_count_reset(moves_to_reset),
    method_of_reset(reset_method),
    whose_turn(starting_turn),
    game_start_date_time(previous_start_time)
{
}

Game_Result Clock::punch(const Board& board) noexcept
{
    if( ! is_in_use() || ! is_running())
    {
        return {};
    }

    auto time_this_punch = std::chrono::steady_clock::now();

    auto player_index = static_cast<int>(whose_turn);
    timers[player_index] -= (time_this_punch - time_previous_punch);
    time_previous_punch = time_this_punch;
    whose_turn = opposite(whose_turn);

    auto punch_result = Game_Result{};

    if(time_left(opposite(whose_turn)) < 0.0s)
    {
        if(board.enough_material_to_checkmate(whose_turn))
        {
            punch_result = Game_Result(whose_turn, Game_Result_Type::TIME_FORFEIT);
        }
        else
        {
            punch_result = Game_Result(Winner_Color::NONE, Game_Result_Type::TIME_EXPIRED_WITH_INSUFFICIENT_MATERIAL);
        }
    }

    if(++moves_since_clock_reset[player_index] == move_count_reset)
    {
        if(method_of_reset == Time_Reset_Method::ADDITION)
        {
            timers[player_index] += initial_start_time;
        }
        else
        {
            timers[player_index] = initial_start_time;
        }
        moves_since_clock_reset[player_index] = 0;
    }
    timers[player_index] += increment_time[player_index];

    return punch_result;
}

void Clock::unpunch() noexcept
{
    moves_since_clock_reset[static_cast<int>(whose_turn)] -= 1;
    moves_since_clock_reset[static_cast<int>(opposite(whose_turn))] -= 1;
    punch({});
}

void Clock::stop() noexcept
{
    auto time_stop = std::chrono::steady_clock::now();
    timers[static_cast<int>(whose_turn)] -= (time_stop - time_previous_punch);
    clocks_running = false;
}

void Clock::start() noexcept
{
    static const auto default_game_start_date_time = std::chrono::system_clock::time_point{};
    time_previous_punch = std::chrono::steady_clock::now();
    if(game_start_date_time == default_game_start_date_time)
    {
        game_start_date_time = std::chrono::system_clock::now();
    }
    clocks_running = true;
}

Clock::seconds Clock::time_left(Piece_Color color) const noexcept
{
    if( ! is_in_use())
    {
        return 0.0s;
    }

    if(whose_turn != color || ! clocks_running)
    {
        return timers[static_cast<int>(color)];
    }
    else
    {
        auto now = std::chrono::steady_clock::now();
        return timers[static_cast<int>(color)] - (now - time_previous_punch);
    }
}

size_t Clock::moves_until_reset(Piece_Color color) const noexcept
{
    if(move_count_reset > 0)
    {
        return move_count_reset - moves_since_clock_reset[static_cast<int>(color)];
    }
    else
    {
        return std::numeric_limits<int>::max();
    }
}

Piece_Color Clock::running_for() const noexcept
{
    return whose_turn;
}

void Clock::set_time(Piece_Color player, seconds new_time_seconds) noexcept
{
    timers[static_cast<int>(player)] = new_time_seconds;
    time_previous_punch = std::chrono::steady_clock::now();
}

void Clock::set_increment(Piece_Color player, seconds new_increment_time_seconds) noexcept
{
    increment_time[static_cast<int>(player)] = new_increment_time_seconds;
}

void Clock::set_next_time_reset(Piece_Color player, size_t moves_to_reset) noexcept
{
    if(moves_to_reset == 0)
    {
        move_count_reset = 0;
    }
    else
    {
        move_count_reset = moves_since_clock_reset[static_cast<int>(player)] + moves_to_reset;
    }
}

Clock::seconds Clock::running_time_left() const noexcept
{
    return time_left(running_for());
}

bool Clock::is_running() const noexcept
{
    return clocks_running;
}

std::chrono::system_clock::time_point Clock::game_start_date_and_time() const noexcept
{
    return game_start_date_time;
}

size_t Clock::moves_per_time_period() const noexcept
{
    return move_count_reset;
}

Time_Reset_Method Clock::reset_mode() const noexcept
{
    return method_of_reset;
}

Clock::seconds Clock::initial_time() const noexcept
{
    return initial_start_time;
}

Clock::seconds Clock::increment(Piece_Color color) const noexcept
{
    return increment_time[static_cast<int>(color)];
}

bool Clock::is_in_use() const noexcept
{
    return initial_time() > 0.0s || increment(Piece_Color::WHITE) > 0.0s || increment(Piece_Color::BLACK) > 0.0s;
}

std::string Clock::time_control_string() const noexcept
{
    std::ostringstream time_control_spec;
    if(moves_per_time_period() > 0)
    {
        time_control_spec << moves_per_time_period() << '/';
    }
    time_control_spec << initial_time().count();
    if(increment(Piece_Color::WHITE) > 0.0s)
    {
        time_control_spec << '+' << increment(Piece_Color::WHITE).count();
    }
    return time_control_spec.str();
}
