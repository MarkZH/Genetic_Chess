#include "Players/Player.h"

#include <string>

class Game_Result;
class Board;

void Player::process_game_ending(const Game_Result&, const Board&) const
{
}

std::string Player::author() const
{
    return "";
}

std::string Player::get_commentary_for_move(size_t) const
{
    return "";
}

void Player::initial_board_setup(Board&) const
{
}
