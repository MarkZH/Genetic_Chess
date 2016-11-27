#ifndef GAME_H
#define GAME_H

#include <string>

#include "Color.h"

class Player;
class Board;

Color play_game(const Player& white,
                const Player& black,
                int time_in_seconds,
                int moves_to_reset_clock,
                const std::string& pgn_file_name);

#endif // GAME_H
