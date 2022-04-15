#ifndef ALPHA_BETA_VALUE_H
#define ALPHA_BETA_VALUE_H

#include <utility>

#include "Game/Color.h"

//! \brief A class for tracking Alpha and Beta values through a minimax search.
//!
//! This class is lighter weight than a Game_Tree_Node_Result since it does
//! not contain the std::vector<const Move*> of the variation.
class Alpha_Beta_Value
{
    public:
        //! \brief Direct constructor of a value
        //!
        //! \param score_in The board score.
        //! \param perspective_in The player perspective from which the score is derived.
        //! \param depth_in The depth of the search that produced the score.
        Alpha_Beta_Value(double score_in, Piece_Color perspective_in, size_t depth_in) noexcept;

        //! \brief The depth of search that created this value
        size_t depth() const noexcept;

        //! \brief Does this value represent a winning position?
        //!
        //! \param player_color The player for whom this value may represent a win.
        bool is_winning_for(Piece_Color player_color) const noexcept;

        //! \brief A comparable value for ordering Alpha_Beta_Values and Game_Tree_Node_Results
        //!
        //! \param player_color The perspective from which to order values.
        //!
        //! See Game_Tree_Node_Result::value() for details
        std::pair<double, int> value(Piece_Color player_color) const noexcept;

        //! \brief Create a value for alpha at the beginning of a minimax search with alpha-beta pruning.
        //! 
        //! \param perspective The color of the player about to start the search.
        static Alpha_Beta_Value alpha_start(Piece_Color perspective) noexcept;

        //! \brief Create a value for beta at the beginning of a minimax search with alpha-beta pruning.
        //! 
        //! \param perspective The color of the player about to start the search.
        static Alpha_Beta_Value beta_start(Piece_Color perspective) noexcept;

    private:
        double score;
        Piece_Color perspective;
        size_t variation_depth;
};

#endif // ALPHA_BETA_VALUE_H
