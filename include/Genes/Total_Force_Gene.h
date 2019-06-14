#ifndef TOTAL_FORCE_GENE_H
#define TOTAL_FORCE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;
class Piece_Strength_Gene;

//! Scores a board based on the weighted sum of all a player's pieces.
//
//! The weights are provided by the Piece_Strength_Gene.
class Total_Force_Gene : public Gene
{
    public:
        //! The Total_Force_Gene constructor requires a Piece_Strength_Gene to reference in score_board().
        //
        //! \param piece_strength_source The source of piece values to weight the importance of various pieces.
        explicit Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source);

        std::unique_ptr<Gene> duplicate() const override;
        void reset_piece_strength_gene(const Piece_Strength_Gene* psg) override;

        std::string name() const override;

    private:
        const Piece_Strength_Gene* piece_strength_source;

        double score_board(const Board& board, Color perspective, size_t depth) const override;
};

#endif // TOTAL_FORCE_GENE_H
