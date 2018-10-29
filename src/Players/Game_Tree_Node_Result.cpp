#include "Players/Game_Tree_Node_Result.h"

#include <cmath>
#include <utility>

#include "Game/Color.h"

double Game_Tree_Node_Result::corrected_score(Color query) const
{
    return query == perspective ? score : -score;
}

std::pair<double, int> Game_Tree_Node_Result::value(Color query) const
{
    auto standardized_score = corrected_score(query);
    return {standardized_score, int(depth())*(standardized_score > 0 ? -1 : 1)};
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
