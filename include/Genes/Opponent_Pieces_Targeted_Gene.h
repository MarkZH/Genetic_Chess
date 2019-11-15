#ifndef OPPONENT_PIECES_TARGETED_GENE_H
#define OPPONENT_PIECES_TARGETED_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Piece_Strength_Gene;
class Board;

//! Scores a board based on a weighted count of how many opponent's pieces are threatened.
//
//! The weights are provided by the Piece_Strength_Gene.
class Opponent_Pieces_Targeted_Gene : public Gene
{
    public:
        //! The Opponent_Pieces_Targeted_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //
        //! \param piece_strength_gene The source of piece values to weight the importance of various pieces.
        explicit Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) noexcept;

        std::unique_ptr<Gene> duplicate() const noexcept override;
        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept override;

        std::string name() const noexcept override;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // OPPONENT_PIECES_TARGETED_GENE_H
