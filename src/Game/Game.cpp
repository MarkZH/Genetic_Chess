#include "Game/Game.h"

#include <string>
#include <stdexcept>

#ifndef _WIN32
#include <csignal>
#endif // _WIN32

#include "Players/Player.h"
#include "Players/Outside_Communicator.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

// Play single game, return color of winner
Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      bool pondering_allowed,
                      const std::string& pgn_file_name)
{
    if(board.whose_turn() != game_clock.running_for())
    {
        throw std::invalid_argument("Board and Clock disagree on whose turn it is.");
    }

    try
    {
        game_clock.start();
        Game_Result result;

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

        board.print_game_record(&white,
                                &black,
                                pgn_file_name,
                                result,
                                game_clock);
        return result;

    }
    catch(const std::exception& game_error)
    {
        board.print_game_record(&white,
                                &black,
                                pgn_file_name,
                                {},
                                game_clock,
                                game_error.what());
        throw;
    }
}

void play_game_with_outsider(const Player& player)
{
    #ifndef _WIN32
    signal(SIGTSTP, SIG_IGN);
    #endif // _WIN32

    auto outsider = connect_to_outside(player);

    Board board;
    Clock clock;

    while(true)
    {
        outsider->setup_turn(board, clock);
        outsider->listen(board, clock);

        if( ! clock.is_running())
        {
            clock.start();
        }
        if(clock.running_for() != board.whose_turn())
        {
            clock.punch();
        }

        const auto& chosen_move = player.choose_move(board, clock);
        clock.punch();

        outsider->handle_move(board, chosen_move);
        player.ponder(board, clock, outsider->pondering_allowed());
    }
}
