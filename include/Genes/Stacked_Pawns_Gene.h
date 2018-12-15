#ifndef STACKED_PAWNS_GENE_H
#define STACKED_PAWNS_GENE_H

#include <memory>
#include <string>

#include "Genes/Gene.h"

class Board;

class Stacked_Pawns_Gene : public Gene
{
    public:
        double score_board(const Board& board, const Board& opposite_board) const override;

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;
};

#endif // STACKED_PAWNS_GENE_H
