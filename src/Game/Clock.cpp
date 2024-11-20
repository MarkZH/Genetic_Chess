#include "Game/Clock.h"

#include <array>
#include <chrono>
using namespace std::chrono_literals;
#include <limits>
#include <sstream>
#include <print>
#include <format>

#include "Game/Board.h"
#include "Game/Game_Result.h"

Clock::Clock(const seconds duration_seconds,
             const size_t moves_to_reset,
             const seconds increment_seconds,
             const Time_Reset_Method reset_method,
             const std::chrono::system_clock::time_point previous_start_time) noexcept :
    timers({duration_seconds, duration_seconds}),
    initial_start_time(duration_seconds),
    increment_time({increment_seconds, increment_seconds}),
    move_count_reset(moves_to_reset),
    method_of_reset(reset_method),
    game_start_date_time(previous_start_time)
{
}

Game_Result Clock::punch(const Board& board) noexcept
{
    if( ! is_running())
    {
        return {};
    }

    const auto time_this_punch = std::chrono::steady_clock::now();

    const auto player_index = std::to_underlying(whose_turn);
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
    moves_since_clock_reset[std::to_underlying(whose_turn)] -= 1;
    moves_since_clock_reset[std::to_underlying(opposite(whose_turn))] -= 1;
    punch({});
}

void Clock::stop() noexcept
{
    if(clocks_running)
    {
        const auto time_stop = std::chrono::steady_clock::now();
        timers[std::to_underlying(whose_turn)] -= (time_stop - time_previous_punch);
        clocks_running = false;
    }
}

void Clock::start(const Piece_Color starting_turn) noexcept
{
    whose_turn = starting_turn;
    time_previous_punch = std::chrono::steady_clock::now();
    if(game_start_date_time == std::chrono::system_clock::time_point{})
    {
        game_start_date_time = std::chrono::system_clock::now();
    }
    clocks_running = initial_start_time > 0s;
}

Clock::seconds Clock::time_left(const Piece_Color color) const noexcept
{
    if(whose_turn != color || ! clocks_running)
    {
        return timers[std::to_underlying(color)];
    }
    else
    {
        const auto now = std::chrono::steady_clock::now();
        return timers[std::to_underlying(color)] - (now - time_previous_punch);
    }
}

size_t Clock::moves_until_reset(const Piece_Color color) const noexcept
{
    if(move_count_reset > 0)
    {
        return move_count_reset - moves_since_clock_reset[std::to_underlying(color)];
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

void Clock::set_time(const Piece_Color player, const seconds new_time_seconds) noexcept
{
    timers[std::to_underlying(player)] = new_time_seconds;
    time_previous_punch = std::chrono::steady_clock::now();
}

void Clock::set_increment(const Piece_Color player, const seconds new_increment_time_seconds) noexcept
{
    increment_time[std::to_underlying(player)] = new_increment_time_seconds;
}

void Clock::set_next_time_reset(const Piece_Color player, const size_t moves_to_reset) noexcept
{
    if(moves_to_reset == 0)
    {
        move_count_reset = 0;
    }
    else
    {
        move_count_reset = moves_since_clock_reset[std::to_underlying(player)] + moves_to_reset;
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

Clock::seconds Clock::increment(const Piece_Color color) const noexcept
{
    return increment_time[std::to_underlying(color)];
}

std::string Clock::time_control_string() const noexcept
{
    std::ostringstream time_control_spec;

    if(moves_per_time_period() > 0)
    {
        std::print(time_control_spec, "{}/", moves_per_time_period());
    }

    if(initial_time() > 0s)
    {
        std::print(time_control_spec, "{}", initial_time().count());
    }
    
    if(increment(Piece_Color::WHITE) > 0.0s)
    {
        std::print(time_control_spec, "+{}", increment(Piece_Color::WHITE).count());
    }
    
    const auto time_spec = time_control_spec.str();
    return time_spec.empty() ? "-" : time_spec;
}

bool Clock::time_expired(Piece_Color color) const noexcept
{
    return time_left(color) <= Clock::seconds{0};
}

bool Clock::running_time_expired() const noexcept
{
    return time_expired(running_for());
}

std::string Clock::time_left_display(Piece_Color color) const noexcept
{
    const auto time = time_left(color);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(time).count();
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time).count() % 60;
    const auto seconds = std::fmod(std::floor(10*time.count())/10, 60.0);
    return std::format("{:02}:{:02}:{:04.1f}", hours, minutes, seconds);
}
