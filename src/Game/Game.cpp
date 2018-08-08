#include "Game/Game.h"

#include <fstream>
#include <string>
#include <mutex>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Players/Thinking.h"
#include "Exceptions/Game_Ended.h"

// Play single game, return color of winner
Game_Result play_game(const Player& white,
                      const Player& black,
                      double time_in_seconds,
                      size_t moves_to_reset,
                      double increment_seconds,
                      const std::string& pgn_file_name)
{
    Board board;

    white.initial_board_setup(board);
    black.initial_board_setup(board);

    return play_game_with_board(white,
                                black,
                                time_in_seconds,
                                moves_to_reset,
                                increment_seconds,
                                pgn_file_name,
                                board);
}

Game_Result play_game_with_board(const Player& white,
                                 const Player& black,
                                 double time_in_seconds,
                                 size_t moves_to_reset,
                                 double increment_seconds,
                                 const std::string& pgn_file_name,
                                 Board& board)
{
    static std::mutex write_lock;

    Clock game_clock(time_in_seconds, moves_to_reset, increment_seconds);

    try
    {
        game_clock.start();
        Game_Result result;
        std::string last_move;

        try
        {
            while(true)
            {
                auto& player = board.whose_turn() == WHITE ? white : black;
                const auto& move_chosen = player.choose_move(board, game_clock);
                result = game_clock.punch();
                if(result.game_has_ended())
                {
                    last_move = move_chosen.coordinate_move();
                    break;
                }

                result = board.submit_move(move_chosen);
                if(result.game_has_ended())
                {
                    break;
                }
            }
        }
        catch(const Game_Ended& gee)
        {
            // Only occurs for GUI communication errors
            result = {gee.winner(), gee.what()};
        }

        // for Outside_Players communicating with xboard and the like
        white.process_game_ending(result, board, last_move);
        black.process_game_ending(result, board, last_move);

        std::lock_guard<std::mutex> write_lock_guard(write_lock);

        board.print_game_record(&white,
                                &black,
                                pgn_file_name,
                                result,
                                time_in_seconds,
                                moves_to_reset,
                                increment_seconds,
                                game_clock);

        return result;
    }
    catch(const std::exception& error)
    {
        std::lock_guard<std::mutex> write_lock_guard(write_lock);
        board.ascii_draw(WHITE);
        board.print_game_record(&white,
                                &black,
                                pgn_file_name,
                                Game_Result(NONE, error.what()),
                                time_in_seconds,
                                moves_to_reset,
                                increment_seconds,
                                game_clock);
        throw;
    }
}
