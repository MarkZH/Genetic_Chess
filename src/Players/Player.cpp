#include "Players/Player.h"

#include <string>

class Game_Ending_Exception;

Player::~Player()
{
}

void Player::process_game_ending(const Game_Ending_Exception&, const Board&) const
{
}

std::string Player::author() const
{
    return "";
}
