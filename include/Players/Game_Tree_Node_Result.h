#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <string>

#include "Game/Color.h"

class Move;

struct Game_Tree_Node_Result
{
    double score;
    Color perspective;
    std::vector<const Move*> variation;

    double corrected_score(Color query) const;
    size_t depth() const;
    bool is_winning_for(Color query) const;
    bool is_losing_for(Color query) const;
};

// Is "a" a better result than "b" from "perspective" color
bool better_than(const Game_Tree_Node_Result& a,
                 const Game_Tree_Node_Result& b,
                 Color perspective);

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b);

#endif // GAME_TREE_NODE_RESULT_H
