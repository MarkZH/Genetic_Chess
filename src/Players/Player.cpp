#include "Players/Player.h"

#include <string>

class Board;

bool Player::use_short_cecp_post = false;

void Player::reset() const noexcept
{
}

std::string Player::commentary_for_next_move(const Board&, size_t) const noexcept
{
    return {};
}

void Player::undo_move(const Move*) const noexcept
{
}

void Player::set_short_post() noexcept
{
    use_short_cecp_post = true;
}

bool Player::use_short_post() noexcept
{
    return use_short_cecp_post;
}
