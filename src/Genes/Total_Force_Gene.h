#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;
class Piece_Strength_Gene;

//! \brief Scores a board based on the weighted sum of all a player's pieces.
//!
//! The weights are provided by the Piece_Strength_Gene.
class Total_Force_Gene : public Clonable_Gene<Total_Force_Gene>
{
    public:
        //! \brief The Total_Force_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //!
        //! \param piece_strength_source The source of piece values to weight the importance of various pieces.
        explicit Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source) noexcept;

        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept override;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // TOTAL_FORCE_GENE_H
