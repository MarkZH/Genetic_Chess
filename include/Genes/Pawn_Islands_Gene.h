#ifndef PAWN_ISLANDS_GENE_H
#define PAWN_ISLANDS_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on the number of isolated pawn groups a player has.
class Pawn_Islands_Gene : public Clonable_Gene<Pawn_Islands_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // PAWN_ISLANDS_GENE_H
