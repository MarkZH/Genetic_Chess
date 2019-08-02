#include "Players/Player.h"

#include <string>

class Board;

bool Player::use_short_cecp_post = false;

void Player::ponder(const Board&, const Clock&, bool) const
{
}

std::string Player::commentary_for_next_move(const Board&, size_t) const
{
    return {};
}

void Player::set_opponent_name(const std::string& name) const
{
    opposing_player_name = name;
}

std::string Player::opponent_name() const
{
    return opposing_player_name;
}

void Player::set_short_post()
{
    use_short_cecp_post = true;
}

bool Player::use_short_post()
{
    return use_short_cecp_post;
}
