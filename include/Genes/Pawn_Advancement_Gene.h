#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on how close the pawns are to promotion.
class Pawn_Advancement_Gene : public Clonable_Gene<Pawn_Advancement_Gene>
{
    public:
        Pawn_Advancement_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
