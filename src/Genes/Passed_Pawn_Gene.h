#ifndef PASSED_PAWN_GENE_H
#define PASSED_PAWN_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on how many passed pawns (pawns with no opponent pawns ahead of them or in adjacent files) a player has.
class Passed_Pawn_Gene : public Clonable_Gene<Passed_Pawn_Gene>
{
    public:
        Passed_Pawn_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // PASSED_PAWN_GENE_H
