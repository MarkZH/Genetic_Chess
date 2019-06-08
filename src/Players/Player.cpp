#include "Players/Player.h"

#include <string>

class Board;

void Player::ponder(const Board&, const Clock&, bool) const
{
}

std::string Player::commentary_for_next_move(const Board&, size_t) const
{
    return {};
}
