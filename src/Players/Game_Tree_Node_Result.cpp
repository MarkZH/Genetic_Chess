#include "Players/Game_Tree_Node_Result.h"

#include <cmath>

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
    if(scoreA == Math::win_score)
    {
        return a.depth < b.depth;
    }

    // Longer path to losing is better
    if(scoreA == Math::lose_score)
    {
        return a.depth > b.depth;
    }

    return false;
}

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b)
{
    auto scoreA = a.corrected_score(WHITE);
    auto scoreB = b.corrected_score(WHITE);

    if(scoreA != scoreB)
    {
        return false;
    }

    // scoreA == scoreB

    if(std::abs(scoreA) == Math::win_score)
    {
        return a.depth == b.depth;
    }

    return true;
}
