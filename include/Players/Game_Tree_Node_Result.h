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

    //! How much time was allotted for picking this move.
    double time_allotted = 0.0;
    //! How much time was actually usd in picking this move.
    double time_used = 0.0;

    //! The score assigned to a checkmate board position for the winning Minimax_AI.
    static const double win_score;
    //! The score assigned to a checkmate board position for the losing Minimax_AI.
    static const double lose_score;

    //! Gives the score of the board position from the indicated side.
    //
    //! The score is calculated in a way that opposite perspective scores
    //! have opposite sign.
    //! \param query The color of the player for whom the value of the board is sought.
    double corrected_score(Color query) const;

    //! The depth in the game tree where this result was calculated.
    //
    //! \returns The number of moves from the original board needed to reach
    //!          the evaluated board.
    size_t depth() const;

    //! Determine whether the result represents a winning endgame for a player.
    //
    //! \param query The player for whom the result may be a win.
    bool is_winning_for(Color query) const;

    //! Determine whether the result represents a losing endgame for a player.
    //
    //! \param query The player for whom the result may be a loss.
    bool is_losing_for(Color query) const;

    //! Creates a quantity that can be compared with other Game_Tree_Node_Results
    //
    //! The quanity considers both the score and the depth in the following steps:
    //! 1. If the quantity respresents a win, then it compares greater than any non-win result.
    //! 2. If the quantity it compares to is also a win, then the shallower depth (faster win) is preferred.
    //! 3. If the quantity represents a loss, then it compares less than any non-loss result.
    //! 3. If both quantities represent losses, then the greater depth (slower loss) is preferred.
    //! 4. Otherwise, the higher score prevails.
    //! \param query The color of the player whose perspective is being considered.
    //! \returns A comparable quantity for picking the best result according to the above steps.
    std::pair<double, int> value(Color query) const;
};

#endif // GAME_TREE_NODE_RESULT_H
