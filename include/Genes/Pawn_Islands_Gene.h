#ifndef PAWN_ISLANDS_GENE_H
#define PAWN_ISLANDS_GENE_H

#include "Gene.h"

#include <memory>
#include <string>

#include "Game/Color.h"

class Board;

//! Scores a board based on the number of isolated pawn groups a player has.
class Pawn_Islands_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective, size_t depth) const override;
};

#endif // PAWN_ISLANDS_GENE_H
