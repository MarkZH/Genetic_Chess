#include "Game/Game.h"

#include <string>
#include <optional>
#include <stdexcept>

#include "Players/Player.h"
#include "Players/Outside_Communicator.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Exceptions/Game_Ended.h"

// Play single game, return color of winner
Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      bool pondering_allowed,
                      const std::string& pgn_file_name)
{
    game_clock.start();

    Game_Result result;
    std::optional<std::exception> error;

    try
    {
        while( ! result.game_has_ended())
        {
            auto& player  = board.whose_turn() == WHITE ? white : black;
            auto& thinker = board.whose_turn() == WHITE ? black : white;

            thinker.ponder(board, game_clock, pondering_allowed);
            const auto& move_chosen = player.choose_move(board, game_clock);

            result = game_clock.punch();
            if( ! result.game_has_ended())
            {
                result = board.submit_move(move_chosen);
            }
        }
    }
    catch(const Game_Ended& termination)
    {
        result = termination.result();
    }
    catch(const std::exception& other_error)
    {
        result = {NONE, other_error.what(), false};
        error = other_error;
    }

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

void play_game_with_outsider(const Player& player)
{
    auto outsider = connect_to_outside(player);

    Board board;
    Clock clock;

    while(true)
    {
        outsider->setup_turn(board, clock);
        outsider->listen(board, clock);
        const auto& chosen_move = player.choose_move(board, clock);
        outsider->handle_move(board, clock, chosen_move);
        player.ponder(board, clock, outsider->pondering_allowed());
    }
}
