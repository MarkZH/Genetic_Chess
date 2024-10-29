#include "Players/Player.h"

#include <string>
#include <atomic>

#include "Players/Thinking.h"

class Board;

namespace
{
    std::atomic<Thinking_Output_Type> thinking_indicator = Thinking_Output_Type::NO_THINKING;
    std::atomic_bool move_immediately = false;
}

void Player::reset() const noexcept
{
}

std::string Player::commentary_for_next_move(const Board&) const noexcept
{
    return {};
}

void Player::undo_move(const Move*) const noexcept
{
}

void Player::set_thinking_mode(const Thinking_Output_Type mode) noexcept
{
    thinking_indicator = mode;
}

Thinking_Output_Type Player::thinking_mode() noexcept
{
    return thinking_indicator;
}

void Player::pick_move_now() noexcept
{
    move_immediately = true;
}

void Player::choose_move_at_leisure() noexcept
{
    move_immediately = false;
}

void Player::add_win() noexcept
{
    ++win_count;
}

int Player::wins() const noexcept
{
    return win_count;
}

bool Player::must_pick_move_now() noexcept
{
    return move_immediately;
}

void Player::add_draw() noexcept
{
    ++draw_count;
}

int Player::draws() const noexcept
{
    return draw_count;
}
