#include "Players/Game_Tree_Node_Result.h"

#include <cmath>
#include <cassert>

#include "Game/Color.h"
#include "Utility.h"

double Game_Tree_Node_Result::corrected_score(Color query) const
{
    return query == perspective ? score : -score;
}

bool better_than(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b, Color perspective)
{
    auto scoreA = a.corrected_score(perspective);
    auto scoreB = b.corrected_score(perspective);

    if(scoreA > scoreB)
    {
        return true;
    }

    if(scoreA < scoreB)
    {
        return false;
    }

    // scoreA == scoreB

    // Shorter path to winning is better
    if(a.is_winning_for(perspective))
    {
        return a.depth() < b.depth();
    }

    // Longer path to losing is better
    if(a.is_losing_for(perspective))
    {
        return a.depth() > b.depth();
    }

    return false;
}

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b)
{
    auto scoreA = a.corrected_score(WHITE);
    auto scoreB = b.corrected_score(WHITE);

    if(scoreA > scoreB || scoreA < scoreB)
    {
        return false;
    }

    // scoreA == scoreB

    if(a.is_losing_for(WHITE) || a.is_winning_for(WHITE))
    {
        return a.depth() == b.depth();
    }

    return true;
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
