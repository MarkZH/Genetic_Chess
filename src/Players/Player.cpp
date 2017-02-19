#include "Players/Player.h"

#include "Game/Color.h"
#include "Exceptions/Game_Ending_Exception.h"

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
