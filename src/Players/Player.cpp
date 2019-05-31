#include "Players/Player.h"

#include <string>

class Game_Result;
class Board;

//! Allow the player to do any post-game processing or cleanup.
//
//! This is mostly for Outside_Player instances to send final
//! communications to the GUI.
//! \param ending The result of the last action of the game.
//! \param board The board on which the game was played.
void Player::process_game_ending(const Game_Result&, const Board&) const
{
}

void Player::ponder(const Board&, const Clock&, bool) const
{
}

//! Report the creator of the chess engine.
//
//! \returns The programmer who wrote the chess engine.
std::string Player::author() const
{
    return {};
}

//! Provide commentary for the move to come given a past state of the game.
//
//! \param board The board position prior to the move the player should comment on.
//! \returns A text string commenting on the move that will follow the input position.
std::string Player::commentary_for_next_move(const Board&) const
{
    return {};
}

//! Allow the player to make pre-game adjustments to the board.
//
//! The adjustments are usually made by Outside_Player instances to set
//! the initial thinking format and may, in the future, be used to set
//! up the board for Chess960.
//! \param board The board to adjust.
void Player::initial_board_setup(Board&) const
{
}

//! Indicate whether the in-program clock should stop the game when time expires.
//
//! Most players will return true. Players that communicate with external programs
//! or GUIs will return false so that the external arbiter has final say over the
//! game clock.
//! \returns The player's preference for giving the local clock ultimate control over ending the game.
bool Player::stop_for_local_clock() const
{
    return true;
}

bool Player::off_time_thinking_allowed() const
{
    return true;
}

bool Player::print_game_to_stdout() const
{
    return true;
}
