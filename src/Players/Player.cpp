#include "Players/Player.h"

#include <string>

class Board;

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
