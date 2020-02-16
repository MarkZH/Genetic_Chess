#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Scores a board based on how close the pawns are to promotion.
//
//! This score is modulated by an adjustment that favors the pushing
//! of either advanced pawns or rear pawns.
class Pawn_Advancement_Gene : public Clonable_Gene<Pawn_Advancement_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
