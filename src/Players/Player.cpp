#include "Players/Player.h"

#include <string>

class Board;

void Player::ponder(const Board&, const Clock&, bool) const
{
}

//! Provide commentary for the move to come given a past state of the game.
//
//! \param board The board position prior to the move the player should comment on.
//! \returns A text string commenting on the move that will follow the input position.
std::string Player::commentary_for_next_move(const Board&) const
{
    return {};
}
