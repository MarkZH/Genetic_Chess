#include "Players/Game_Tree_Node_Result.h"

#include <cmath>
#include <utility>
#include <limits>

#include "Game/Color.h"
#include "Utility/Math.h"

const double Game_Tree_Node_Result::win_score = std::numeric_limits<double>::infinity();
const double Game_Tree_Node_Result::lose_score = -win_score;

//! Gives the score of the board position from the indicated side.
//
//! The score is calculated in a way that opposite perspective scores
//! have opposite sign.
//! \param query The color of the player for whom the value of the board is sought.
double Game_Tree_Node_Result::corrected_score(Color query) const
{
    return query == perspective ? score : -score;
}

//! Creates a quantity that can be compared with other Game_Tree_Node_Results
//
//! The quanity considers both the score and the depth in the following steps:
//! 1. If the quantity respresents a win, then it compares greater than any non-win result.
//! 2. If the quantity it compares to is also a win, then the shallower depth (faster win) is preferred.
//! 3. If both quantities represent losses, then the greater depth (slower loss) is preferred.
//! 4. Otherwise, the higher score prevails.
//! \param query The color of the player whose perspective is being considered.
//! \returns A comparable quantity for picking the best result according to the above steps.
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

//! The depth in the game tree where this result was calculated.
//
//! \returns The number of moves from the original board needed to reach
//!          the evaluated board.
size_t Game_Tree_Node_Result::depth() const
{
    return variation.size();
}

//! Determine whether the result represents a winning endgame for a player.
//
//! \param query The player for whom the result may be a win.
bool Game_Tree_Node_Result::is_winning_for(Color query) const
{
    return std::isinf(score) && ((score > 0) == (query == perspective));
}

//! Determine whether the result represents a losing endgame for a player.
//
//! \param query The player for whom the result may be a loss.
bool Game_Tree_Node_Result::is_losing_for(Color query) const
{
    return std::isinf(score) && ((score < 0) == (query == perspective));
}
