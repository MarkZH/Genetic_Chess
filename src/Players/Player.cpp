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

std::string Player::commentary_for_next_move(const Board&) const
{
    return {};
}

void Player::initial_board_setup(Board&) const
{
}

bool Player::stop_for_local_clock() const
{
    return true;
}
