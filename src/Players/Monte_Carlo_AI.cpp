#include "Players/Monte_Carlo_AI.h"

#include <vector>
#include <limits>
#include <iostream>

#include "Players/Random_AI.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility.h"

const Move& Monte_Carlo_AI::choose_move(const Board& board, const Clock& clock) const
{
    auto choice_time_start = clock.time_left(clock.running_for());
    auto moves_left_in_game = size_t(Math::average_moves_left(50, 0.5, board.game_record().size()/2));
    auto moves_to_reset = clock.moves_to_reset(clock.running_for());
    auto moves_left = std::min(moves_left_in_game, moves_to_reset);
    auto time_to_examine = clock.time_left(clock.running_for())/moves_left;
    auto time_for_each_move = 0.99*time_to_examine/board.legal_moves().size();

    auto best_move = board.legal_moves().front();
    auto best_result = std::numeric_limits<int>::lowest();
    int game_count = 0;

    for(auto move : board.legal_moves())
    {
        auto next_board = board;
        auto immediate_result = next_board.submit_move(*move);

        if(immediate_result.winner() != NONE) // checkmate
        {
            return *move;
        }

        if(immediate_result.game_has_ended()) // stalemate
        {
            if(best_result < 0)
            {
                best_result = 0;
                best_move = move;
            }
            
            continue;
        }


        int result = 0;
        auto time_start = clock.time_left(clock.running_for());
        while(time_start - clock.time_left(clock.running_for()) < time_for_each_move)
        {
            ++game_count;
            auto monte_carlo_board = next_board;
            Game_Result game_result;
            while( ! game_result.game_has_ended())
            {
                auto next_move = choose_random_move(monte_carlo_board);
                game_result = monte_carlo_board.submit_move(*next_move);
            }
            if(game_result.winner() == board.whose_turn())
            {
                result += 1;
            }
            else if(game_result.winner() == opposite(board.whose_turn()))
            {
                result -= 1;
            }
        }

        if(result > best_result)
        {
            best_result = result;
            best_move = move;

            if(board.thinking_mode() == CECP)
            {
                auto time_so_far = choice_time_start - clock.time_left(clock.running_for());
                std::cout << 1 // thought depth
                    << " "
                    << best_result*100 // score
                    << " "
                    << int(time_so_far*100) // search time in centiseconds
                    << " "
                    << game_count // "nodes" searched
                    << " "
                    << 0 // maximum depth (N/A)
                    << " "
                    << int(game_count/time_so_far) // search speed
                    << "\t"
                    << move->game_record_item(board) // best move so far
                    << std::endl;
            }
        }
    }

    return *best_move;
}

std::string Monte_Carlo_AI::name() const
{
    return "Monte Carlo AI";
}
