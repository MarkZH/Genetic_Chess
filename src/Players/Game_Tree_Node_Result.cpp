#include "Players/Game_Tree_Node_Result.h"

#include <cmath>
#include <utility>
#include <limits>

#include "Game/Color.h"
#include "Utility/Math.h"

const double Game_Tree_Node_Result::win_score = std::numeric_limits<double>::infinity();
const double Game_Tree_Node_Result::lose_score = -win_score;

double Game_Tree_Node_Result::corrected_score(Color query) const
{
    return query == perspective ? score : -score;
}

std::pair<double, int> Game_Tree_Node_Result::value(Color query) const
{
    auto standardized_score = corrected_score(query);
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

size_t Game_Tree_Node_Result::depth() const
{
    return variation.size();
}

bool Game_Tree_Node_Result::is_winning_for(Color query) const
{
    return std::isinf(score) && ((score > 0) == (query == perspective));
}

bool Game_Tree_Node_Result::is_losing_for(Color query) const
{
    return std::isinf(score) && ((score < 0) == (query == perspective));
}
