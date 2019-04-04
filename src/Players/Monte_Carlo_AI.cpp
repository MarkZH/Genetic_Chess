#include "Players/Monte_Carlo_AI.h"

#include <vector>
#include <limits>
#include <iostream>
#include <iterator>

#include "Players/Random_AI.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Utility/Math.h"

//! Builds up a sample of the game tree with random complete games to estimate the best move.

//! As the AI plays more random complete games, it fills out the game tree. After some
//! amount of time (estimated to give equal time to every move in the game) it picks the
//! move that most often leads to victory. During the next move, the branches of the
//! game tree not taken are cutoff to keep memory use under control.
//! \param board The current state of the game board.
//! \param clock The game clock.
const Move& Monte_Carlo_AI::choose_move(const Board& board, const Clock& clock) const
{
    auto time_start = clock.running_time_left();
    auto moves_left_in_game = size_t(Math::average_moves_left(50, 0.5, board.game_record().size()/2));
    auto moves_to_reset = clock.moves_to_reset(clock.running_for());
    auto moves_left = std::min(moves_left_in_game, moves_to_reset);
    auto time_to_examine = clock.running_time_left()/moves_left;

    int move_count = 0;
    int game_count = 0;

    auto time_at_last_cecp_output = clock.running_time_left();

    // Prune game tree according to opponent's last move
    if( ! board.game_record().empty())
    {
        search_tree.reroot(board.game_record().back());
    }

    auto search_time_start = clock.running_time_left();
    const Monte_Carlo_Search_Tree* current_search_tree = &search_tree;
    while(time_start - clock.running_time_left() < time_to_examine)
    {
        auto monte_carlo_board = board;
        Game_Result game_result;
        while( ! game_result.game_has_ended())
        {
            ++move_count;
            auto next_move = current_search_tree ? current_search_tree->next_move(monte_carlo_board) : choose_random_move(monte_carlo_board);
            game_result = monte_carlo_board.submit_move(*next_move);
            current_search_tree = current_search_tree ? current_search_tree->subtree(next_move) : nullptr;
        }
        ++game_count;

        int score;
        if(game_result.winner() == NONE)
        {
            score = 0;
        }
        else if(game_result.winner() == board.whose_turn())
        {
            score = 1;
        }
        else
        {
            score = -1;
        }

        auto begin = monte_carlo_board.game_record().begin() + board.game_record().size();
        auto end = monte_carlo_board.game_record().end();
        search_tree.add_search(begin, end, score);

        auto current_time = clock.running_time_left();
        if(board.thinking_mode() == CECP &&
           time_at_last_cecp_output - current_time > 0.1)
        {
            time_at_last_cecp_output = current_time;
            print_cecp_thinking(time_start - current_time,
                                search_time_start - current_time,
                                search_tree.current_score(*begin),
                                move_count,
                                game_count,
                                *begin,
                                board);
        }
    }

    auto best_result = search_tree.best_result();
    if( ! best_result.first)
    {
        best_result.first = choose_random_move(board);
    }
    search_tree.reroot(best_result.first);

    auto current_time = clock.running_time_left();
    if(board.thinking_mode() == CECP)
    {
        print_cecp_thinking(time_start - current_time,
                            search_time_start - current_time,
                            best_result.second,
                            move_count,
                            game_count,
                            best_result.first,
                            board);
    }

    return *best_result.first;
}

//! The name is the same as the algorithm.

//! \returns "Monte Carlo AI"
std::string Monte_Carlo_AI::name() const
{
    return "Monte Carlo AI";
}

void Monte_Carlo_AI::print_cecp_thinking(double time_so_far,
                                         double search_time,
                                         double result,
                                         int move_count,
                                         int game_count,
                                         const Move* move,
                                         const Board& board) const
{
    std::cout << 1 // thought depth
        << " "
        << int(result*100) // score
        << " "
        << int(time_so_far*100) // search time in centiseconds
        << " "
        << move_count // "nodes" searched
        << " "
        << game_count // maximum depth (N/A)
        << " "
        << int(move_count/search_time) // search speed
        << "\t"
        << move->game_record_item(board) // best move so far
        << std::endl;
}
