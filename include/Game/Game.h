#ifndef GAME_H
#define GAME_H

#include <string>

class Player;
class Board;
class Game_Result;

//! \file

//! Play a game with a new board in the default starting state.
//
//! \param white The player with the white pieces.
//! \param black The player with the black pieces.
//! \param time_in_seconds The initial time in seconds on the game clocks.
//! \param moves_to_reset_clock The number of moves to play before the the initial
//!        time is added to the clock.
//! \param increment_seconds The amount of time in seconds to add to the clock after each move.
//! \param pgn_file_name The name of the file where the game record will be written. If empty,
//!        the game will be written to stdout.
//! \returns The result of the game.
Game_Result play_game(const Player& white,
                      const Player& black,
                      double time_in_seconds,
                      size_t moves_to_reset_clock,
                      double increment_seconds,
                      const std::string& pgn_file_name);

//! Play a game with a supplied board.
//
//! \param white The player with the white pieces.
//! \param black The player with the black pieces.
//! \param time_in_seconds The initial time in seconds on the game clocks.
//! \param moves_to_reset_clock The number of moves to play before the the initial
//!        time is added to the clock.
//! \param increment_seconds The amount of time in seconds to add to the clock after each move.
//! \param pgn_file_name The name of the file where the game record will be written. If empty,
//!        the game will be written to stdout.
//! \param board The previously set up board on which the game will be played.
//! \returns The result of the game.
Game_Result play_game_with_board(const Player& white,
                                 const Player& black,
                                 double time_in_seconds,
                                 size_t moves_to_reset_clock,
                                 double increment_seconds,
                                 const std::string& pgn_file_name,
                                 Board& board);
#endif // GAME_H
