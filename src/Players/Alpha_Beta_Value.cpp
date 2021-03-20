#include "Players/Alpha_Beta_Value.h"

#include <cmath>
#include <utility>

#include "Players/Game_Tree_Node_Result.h"
#include "Game/Color.h"
#include "Utility/Math.h"

Alpha_Beta_Value::Alpha_Beta_Value(double score_in, Piece_Color perspective_in, size_t depth_in) noexcept :
    score(score_in),
    perspective(perspective_in),
    variation_depth(depth_in),
    is_draw(false)
{
}

Alpha_Beta_Value& Alpha_Beta_Value::operator=(const Game_Tree_Node_Result& node_value) noexcept
{
    score = node_value.score;
    perspective = node_value.perspective;
    variation_depth = node_value.depth();
    is_draw = node_value.is_draw;
    return *this;
}

size_t Alpha_Beta_Value::depth() const noexcept
{
    return variation_depth;
}

bool Alpha_Beta_Value::is_winning_for(Piece_Color player_color) const noexcept
{
    return std::isinf(score) && ((score > 0) == (player_color == perspective));
}

std::pair<double, int> Alpha_Beta_Value::value(Piece_Color player_color) const noexcept
{
    auto this_score = perspective == player_color || is_draw ? score : -score;
    if(std::isinf(this_score))
    {
        return {this_score, -Math::sign(this_score)*int(depth())};
    }
    else
    {
        return {this_score, 0};
    }
}
