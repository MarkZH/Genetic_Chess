#ifndef STACKED_PAWNS_GENE_H
#define STACKED_PAWNS_GENE_H

#include "Genes/Gene.h"

#include <memory>
#include <string>

#include "Game/Color.h"

class Board;

//! Scores a board based on the number of pawns with friendly pawns in front of them.
class Stacked_Pawns_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const noexcept override;

        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // STACKED_PAWNS_GENE_H
