#include "Players/Game_Tree_Node_Result.h"

#include <cmath>
#include <utility>

#include "Game/Color.h"
#include "Utility/Math.h"
#include "Players/Alpha_Beta_Value.h"

Game_Tree_Node_Result::Game_Tree_Node_Result(const double score_in, const Piece_Color perspective_in, const std::vector<const Move*>& variation_in) noexcept :
    score(score_in),
    perspective(perspective_in),
    variation(variation_in)
{
}

double Game_Tree_Node_Result::corrected_score(const Piece_Color query) const noexcept
{
    return query == perspective ? score : -score;
}

std::pair<double, int> Game_Tree_Node_Result::value(const Piece_Color query) const noexcept
{
    const auto standardized_score = corrected_score(query);
    if(std::isinf(standardized_score))
    {
        // standardized_score == +infinity means a shallower depth
        // is better, and vice versa for -infinity,
        // so make the depth the opposite sign of the score
        return {standardized_score,
                -Math::sign(standardized_score)*int(depth())};
    }
    else
    {
        // For non-winning results, the depth doesn't matter
        return {standardized_score, 0};
    }
}

const std::vector<const Move*>& Game_Tree_Node_Result::variation_line() const noexcept
{
    return variation;
}

Game_Tree_Node_Result::operator bool() const noexcept
{
    return ! variation_line().empty();
}

Alpha_Beta_Value Game_Tree_Node_Result::alpha_beta_value() const noexcept
{
    return {score, perspective, depth()};
}

size_t Game_Tree_Node_Result::depth() const noexcept
{
    return variation.size();
}

bool Game_Tree_Node_Result::is_winning_for(const Piece_Color query) const noexcept
{
    return std::isinf(score) && ((score > 0) == (query == perspective));
}

bool Game_Tree_Node_Result::is_losing_for(const Piece_Color query) const noexcept
{
    return is_winning_for(opposite(query));
}
