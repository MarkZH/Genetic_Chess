#ifndef OPPONENT_PIECES_TARGETED_GENE_H
#define OPPONENT_PIECES_TARGETED_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Piece_Strength_Gene;
class Board;

//! \brief Scores a board based on a weighted count of how many opponent's pieces are threatened.
//!
//! The weights are provided by the Piece_Strength_Gene.
class Opponent_Pieces_Targeted_Gene : public Clonable_Gene<Opponent_Pieces_Targeted_Gene>
{
    public:
        //! \brief The Opponent_Pieces_Targeted_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //!
        //! \param piece_strength_gene The source of piece values to weight the importance of various pieces.
        explicit Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) noexcept;

        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept override;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // OPPONENT_PIECES_TARGETED_GENE_H
