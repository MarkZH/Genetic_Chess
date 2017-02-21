#ifndef GAME_H
#define GAME_H

#include <string>

#include "Color.h"

class Player;
class Board;

// Play a game with a new board in the default starting state.
Color play_game(const Player& white,
                const Player& black,
                double time_in_seconds,
                size_t moves_to_reset_clock,
                double increment_seconds,
                const std::string& pgn_file_name);

// Play a game with a supplied board. Board's state may be modified.
Color play_game_with_board(const Player& white,
                           const Player& black,
                           double time_in_seconds,
                           size_t moves_to_reset_clock,
                           double increment_seconds,
                           const std::string& pgn_file_name,
                           Board& board);
#endif // GAME_H
