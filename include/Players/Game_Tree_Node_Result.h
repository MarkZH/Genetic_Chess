#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <utility>
#include <limits>

#include "Game/Color.h"

class Move;

//! \brief A structure representing the evaluation of a leaf of the Minimax search of the game tree.
struct Game_Tree_Node_Result
{
    //! \brief The score assigned to the board position.
    //!
    //! Victory is scored as positive infinity, defeat as negative infinity,
    //! and a draw as 0.
    double score;

    //! \brief The perspective from which the score is calculated--for whom a higher score is better.
    Piece_Color perspective;

    //! \brief The sequence of moves that lead to the board position being scored.
    std::vector<const Move*> variation;

    //! \brief The score assigned to a checkmate board position for the winning Minimax_AI.
    static constexpr const auto win_score = std::numeric_limits<double>::infinity();
    //! \brief The score assigned to a checkmate board position for the losing Minimax_AI.
    static constexpr const auto lose_score = -win_score;

    //! Indicates the result is a draw, which requires some special handling.
    bool is_draw;

    //! \brief Gives the score of the board position from the indicated side.
    //!
    //! The score is calculated in a way that opposite perspective scores
    //! have opposite sign.
    //! \param query The color of the player for whom the value of the board is sought.
    double corrected_score(Piece_Color query) const noexcept;

    //! \brief The depth in the game tree where this result was calculated.
    //!
    //! \returns The number of moves from the original board needed to reach
    //!          the evaluated board.
    size_t depth() const noexcept;

    //! \brief Determine whether the result represents a winning endgame for a player.
    //!
    //! \param query The player for whom the result may be a win.
    bool is_winning_for(Piece_Color query) const noexcept;

    //! \brief Determine whether the result represents a losing endgame for a player.
    //!
    //! \param query The player for whom the result may be a loss.
    bool is_losing_for(Piece_Color query) const noexcept;

    //! \brief Creates a quantity that can be compared with other Game_Tree_Node_Results
    //!
    //! The quanity considers both the score and the depth in the following steps:
    //! 1. If the quantity respresents a win, then it compares greater than any non-win result.
    //! 2. If the quantity it compares to is also a win, then the shallower depth (faster win) is preferred.
    //! 3. If the quantity represents a loss, then it compares less than any non-loss result.
    //! 3. If both quantities represent losses, then the greater depth (slower loss) is preferred.
    //! 4. Otherwise, the higher score prevails.
    //! \param query The color of the player whose perspective is being considered.
    //! \returns A comparable quantity for picking the best result according to the above steps.
    std::pair<double, int> value(Piece_Color query) const noexcept;
};

#endif // GAME_TREE_NODE_RESULT_H
