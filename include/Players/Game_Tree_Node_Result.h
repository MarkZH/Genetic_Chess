#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <string>

#include "Moves/Complete_Move.h"
#include "Game/Color.h"

struct Game_Tree_Node_Result
{
    Complete_Move move;
    double score;
    Color perspective;
    size_t depth;
    std::vector<Complete_Move> commentary;

    double corrected_score(Color query) const;
};

// Is "a" a better result than "b" from "perspective" color
bool better_than(const Game_Tree_Node_Result& a,
                 const Game_Tree_Node_Result& b,
                 Color perspective);

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b);

#endif // GAME_TREE_NODE_RESULT_H
