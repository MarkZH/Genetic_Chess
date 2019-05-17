#include "Game/Game.h"

#include <string>
#include <optional>
#include <stdexcept>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Exceptions/Game_Ended.h"

// Play single game, return color of winner
Game_Result play_game(Board board,
                      const Player& white,
                      const Player& black,
                      double time_in_seconds,
                      size_t moves_to_reset,
                      double increment_seconds,
                      const std::string& pgn_file_name)
{
    white.initial_board_setup(board);
    black.initial_board_setup(board);

    auto stop_for_clock = white.stop_for_local_clock() && black.stop_for_local_clock();
    Clock game_clock(time_in_seconds, moves_to_reset, increment_seconds, board.whose_turn(), stop_for_clock);
    game_clock.start();

    Game_Result result;
    std::optional<std::exception> error;

    try
    {
        while(true)
        {
            auto& player = board.whose_turn() == WHITE ? white : black;
            const auto& move_chosen = player.choose_move(board, game_clock);

            result = game_clock.punch();
            if(result.game_has_ended())
            {
                break;
            }

            result = board.submit_move(move_chosen);
            if(result.game_has_ended())
            {
                break;
            }
        }
    }
    catch(const Game_Ended& termination)
    {
        result = termination.result();
    }
    catch(const std::exception& other_error)
    {
        result = {NONE, other_error.what()};
        error = other_error;
    }

    // for Outside_Players communicating with xboard and the like
    white.process_game_ending(result, board);
    black.process_game_ending(result, board);

    board.print_game_record(&white,
                            &black,
                            pgn_file_name,
                            result,
                            game_clock);

    if(error)
    {
        throw error.value();
    }
    else
    {
        return result;
    }
}
