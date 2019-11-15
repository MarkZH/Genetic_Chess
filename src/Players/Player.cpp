#include "Players/Player.h"

#include <string>

class Board;

bool Player::use_short_cecp_post = false;

void Player::ponder(const Board&, const Clock&, bool) const noexcept
{
}

std::string Player::commentary_for_next_move(const Board&, size_t) const noexcept
{
    return {};
}

void Player::set_opponent_name(const std::string& name) const noexcept
{
    opposing_player_name = name;
}

std::string Player::opponent_name() const noexcept
{
    return opposing_player_name;
}

void Player::set_short_post() noexcept
{
    use_short_cecp_post = true;
}

bool Player::use_short_post() noexcept
{
    return use_short_cecp_post;
}
