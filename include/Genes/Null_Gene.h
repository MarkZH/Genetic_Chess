#ifndef NULL_GENE_H
#define NULL_GENE_H

#include "Gene.h"

#include <memory>
#include <string>

#include "Game/Color.h"

class Board;

//! A gene that does nothing to show what evolution with no evolutionary pressure looks like.
class Null_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const override;
};

#endif // NULL_GENE_H
