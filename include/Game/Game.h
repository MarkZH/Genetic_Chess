#ifndef GAME_H
#define GAME_H

#include <string>

class Player;
class Board;
class Clock;
class Game_Result;

//! \file

//! Play a game with two local players.
//
//! \param board The Board on which the game will be played.
//! \param game_clock The clock used for time control.
//! \param white The player with the white pieces.
//! \param black The player with the black pieces.
//! \param pondering_allowed Whether AIs are allowed to think during an opponent's move.
//! \param pgn_file_name The name of the file where the game record will be written. If empty,
//!        the game will be written to stdout.
//! \returns The result of the game.
Game_Result play_game(Board board,
                      Clock& game_clock,
                      const Player& white,
                      const Player& black,
                      bool pondering_allowed,
                      const std::string& pgn_file_name);

//! Play a game with an outside GUI interface.
//
//! \param local_player A chess engine chosen at the command line.
//! \param game_file_name The name of a file for writing the game record.
void play_game_with_outsider(const Player& local_player,
                             const std::string& game_file_name);

#endif // GAME_H
