#include "Players/Alpha_Beta_Value.h"

#include <cmath>
#include <utility>

#include "Players/Game_Tree_Node_Result.h"
#include "Game/Color.h"
#include "Utility/Math.h"

Alpha_Beta_Value::Alpha_Beta_Value(const double score_in, const Piece_Color perspective_in, const size_t depth_in) noexcept :
    score(score_in),
    perspective(perspective_in),
    variation_depth(depth_in)
{
}

size_t Alpha_Beta_Value::depth() const noexcept
{
    return variation_depth;
}

bool Alpha_Beta_Value::is_winning_for(const Piece_Color player_color) const noexcept
{
    return std::isinf(score) && ((score > 0) == (player_color == perspective));
}

std::pair<double, int> Alpha_Beta_Value::value(const Piece_Color player_color) const noexcept
{
    const auto this_score = perspective == player_color ? score : -score;
    if(std::isinf(this_score))
    {
        return {this_score, -Math::sign(this_score)*int(depth())};
    }
    else
    {
        return {this_score, 0};
    }
}
