#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <string>
#include <utility>

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
    std::pair<double, int> value(Color query) const;
};

#endif // GAME_TREE_NODE_RESULT_H
