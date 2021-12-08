#ifndef GAME_TREE_NODE_RESULT_H
#define GAME_TREE_NODE_RESULT_H

#include <vector>
#include <utility>
#include <limits>

#include "Game/Color.h"
#include "Players/Alpha_Beta_Value.h"

class Move;

//! \brief A class representing the evaluation of a leaf of the Minimax search of the game tree.
class Game_Tree_Node_Result
{
    public:
        //! \brief The score assigned to a checkmate board position for the winning Minimax_AI.
        static constexpr auto win_score = std::numeric_limits<double>::infinity();
        //! \brief The score assigned to a checkmate board position for the losing Minimax_AI.
        static constexpr auto lose_score = -win_score;
        //! \brief The score assigned to a drawn position.
        static constexpr auto draw_score = 0.0;

        //! \brief Construct an evalutation result.
        //!
        //! \param score_in Victory is scored as positive infinity, defeat as negative infinity,
        //!        and a draw as 0. All other positions take on some finite value.
        //! \param perspective_in The perspective from which the score is calculated--for whom a higher score is better.
        //! \param variation_in The sequence of moves that lead to the board position being scored.
        Game_Tree_Node_Result(double score_in, Piece_Color perspective_in, const std::vector<const Move*>& variation_in) noexcept;

        Game_Tree_Node_Result() noexcept = default;

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

        //! \brief The sequence of moves that lead to the board position being evaluated.
        const std::vector<const Move*>& variation_line() const noexcept;

        //! \brief Whether or not this result has been default constructed
        operator bool() const noexcept;

        //! \brief Returns the equivalent Alpha-Beta cutoff value.
        Alpha_Beta_Value alpha_beta_value() const noexcept;

    private:
        double score = 0.0;
        Piece_Color perspective = Piece_Color::WHITE;
        std::vector<const Move*> variation;
};

#endif // GAME_TREE_NODE_RESULT_H
