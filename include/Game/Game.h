#ifndef GAME_H
#define GAME_H

#include <string>

class Player;
class Board;
class Clock;
class Game_Result;

//! \file

//! \brief Play a game with two local players.
//!
//! \param board The Board on which the game will be played.
//! \param game_clock The clock used for time control.
//! \param white The player with the white pieces.
//! \param black The player with the black pieces.
//! \param event_name The name of the event where the game will take place. May be empty.
//! \param location The name of the location of the game. May be empty.
//! \param pgn_file_name The name of the file where the game record will be written. If empty,
//!        the game will be written to stdout.
//! \param print_board Whether to print the board position to the command line after each move.
//! \returns The result of the game.
Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      const std::string& event_name,
                      const std::string& location,
                      const std::string& pgn_file_name,
                      bool print_board) noexcept;

//! \brief Play a game with an outside GUI interface.
//!
//! \param local_player A chess engine chosen at the command line.
//! \param event_name The name of the event where the game will take place. May be empty.
//! \param location The name of the location of the game. May be empty.
//! \param game_file_name The name of a file for writing the game record. If empty, no record is written.
//! \param enable_logging Log communications (excepting engine thinking) to a file.
void play_game_with_outsider(const Player& local_player,
                             const std::string& event_name,
                             const std::string& location,
                             const std::string& game_file_name,
                             bool enable_logging);

#endif // GAME_H
