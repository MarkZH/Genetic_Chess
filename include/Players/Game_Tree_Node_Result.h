#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <utility>

#include "Game/Color.h"

class Move;

//! A structure representing the evaluation of a leaf of the Minimax search of the game tree.
struct Game_Tree_Node_Result
{
    //! The score assigned to the board position.
    //
    //! Victory is scored as positive infinity, defeat as negative infinity,
    //! and a draw as 0.
    double score;

    //! The perspective from which the score is calculated--for whom a higher score is better.
    Color perspective;

    //! The sequence of moves that lead to the board position being scored.
    std::vector<const Move*> variation;

    double corrected_score(Color query) const;
    size_t depth() const;
    bool is_winning_for(Color query) const;
    bool is_losing_for(Color query) const;
    std::pair<double, int> value(Color query) const;
};

#endif // GAME_TREE_NODE_RESULT_H
