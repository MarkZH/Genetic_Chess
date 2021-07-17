#include "Players/Iterative_Deepening_AI.h"

#include <cmath>
#include <chrono>
#include <vector>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Moves/Move.h"
#include "Players/Game_Tree_Node_Result.h"
#include "Players/Alpha_Beta_Value.h"

#include "Utility/Fixed_Capacity_Vector.h"

std::string Iterative_Deepening_AI::name() const noexcept
{
    return ai_name() + " (Iterative Deepening)";
}

const Move& Iterative_Deepening_AI::choose_move(const Board& board, const Clock& clock) const noexcept
{
    reset_search_stats(board);
    const auto effective_moves_per_turn = branching_factor(game_progress(board));
    const auto time_to_use = time_to_examine(board, clock);
    const auto time_start = std::chrono::steady_clock::now();

    auto principal_variation = std::vector<const Move*>{nullptr, nullptr};
    Game_Tree_Node_Result result;
    for(size_t depth = 1; true; ++depth)
    {
        // alpha = highest score found that opponent will allow
        auto alpha_start = Alpha_Beta_Value{Game_Tree_Node_Result::lose_score,
                                            board.whose_turn(),
                                            0};

        // beta = score that will cause opponent to choose a different prior move
        auto beta_start = Alpha_Beta_Value{Game_Tree_Node_Result::win_score,
                                           board.whose_turn(),
                                           0};

        current_variation_store current_variation;

        result = search_game_tree(board,
                                  Clock::seconds{std::numeric_limits<Clock::seconds::rep>::infinity()},
                                  depth,
                                  depth,
                                  clock,
                                  alpha_start,
                                  beta_start,
                                  principal_variation,
                                  current_variation);

        const auto time_used_so_far = std::chrono::steady_clock::now() - time_start;
        const auto time_left = time_to_use - time_used_so_far;
        if(time_used_so_far*effective_moves_per_turn < time_left)
        {
            principal_variation.resize(2);
            const auto& variation = result.variation_line();
            principal_variation.insert(principal_variation.end(), variation.begin(), variation.end());
        }
        else
        {
            report_final_search_stats(result, board);
            return *result.variation_line().front();
        }
    }
}
