#include "Game/Game.h"

#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <vector>

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

#include "Utility/String.h"

#include "Exceptions/Game_Ended.h"

Game_Result play_game(Board board,
                      Clock game_clock,
                      const Player& white,
                      const Player& black,
                      bool pondering_allowed,
                      const std::string& event_name,
                      const std::string& location,
                      const std::string& pgn_file_name)
{
    if(board.whose_turn() != game_clock.running_for())
    {
        throw std::invalid_argument("Board and Clock disagree on whose turn it is.");
    }

    std::vector<const Move*> game_record;

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

            result = game_clock.punch(board);
            if( ! result.game_has_ended())
            {
                result = board.submit_move(move_chosen);
            }
            game_record.push_back(&move_chosen);
        }

        game_clock.stop();

        board.print_game_record(game_record,
                                &white,
                                &black,
                                pgn_file_name,
                                result,
                                game_clock,
                                event_name,
                                location);
        return result;

    }
    catch(const std::exception& game_error)
    {
        board.print_game_record(game_record,
                                &white,
                                &black,
                                pgn_file_name,
                                {},
                                game_clock,
                                event_name,
                                location,
                                game_error.what());
        throw;
    }
}

void play_game_with_outsider(const Player& player,
                             const std::string& event_name,
                             const std::string& location,
                             const std::string& game_file_name)
{
    auto outsider = connect_to_outside(player);

    Board board;
    Clock clock;
    Game_Result game_result;
    std::vector<const Move*> game_record;
    auto player_color = NONE;

    while(true)
    {
        try
        {
            game_result = outsider->setup_turn(board, clock, game_record, player);
            if(game_result.game_has_ended())
            {
                throw Game_Ended("");
            }
            outsider->listen(board, clock);

            player_color = board.whose_turn();
            const auto& chosen_move = player.choose_move(board, clock);
            clock.punch(board);

            game_result = outsider->handle_move(board, chosen_move, game_record, player);
            if(game_result.game_has_ended())
            {
                throw Game_Ended("");
            }
            player.ponder(board, clock, outsider->pondering_allowed());
        }
        catch(const Game_Ended& game_end)
        {
            std::string reason = game_end.what();
            if( ! reason.empty())
            {
                outsider->log("Game ended with: " + reason);
            }

            auto exit = false;
            if(reason == "quit")
            {
                reason.clear();
                exit = true;
            }

            if( ! game_file_name.empty())
            {
                clock.stop();
                auto white = (player_color == WHITE ? &player : nullptr);
                auto black = (player_color == BLACK ? &player : nullptr);
                board.print_game_record(game_record,
                                        white, black,
                                        String::add_to_file_name(game_file_name, "-" + color_text(player_color)),
                                        game_result,
                                        clock,
                                        event_name,
                                        location,
                                        reason);
            }

            if(exit)
            {
                return;
            }
        }
    }
}
