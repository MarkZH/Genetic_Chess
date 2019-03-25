#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Board;

//! Scores board based on the number of legal moves.
class Freedom_To_Move_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
};

#endif // FREEDOM_TO_MOVE_GENE_H
