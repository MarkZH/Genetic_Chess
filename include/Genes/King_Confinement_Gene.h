#ifndef KING_CONFINEMENT_GENE_H
#define KING_CONFINEMENT_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on how confined the king is by friendly pieces or opponent attacks.
//!
//! This gene uses a flood-fill-like algorithm to count the squares that are reachable by the
//! king from its current positions with unlimited consecutive moves. The boundaries of this
//! area are squares attacked by the other player or occupied by pieces of the same color.
class King_Confinement_Gene : public Clonable_Gene<King_Confinement_Gene>
{
    public:
        King_Confinement_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // KING_CONFINEMENT_GENE_H
