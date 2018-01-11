#ifndef PASSED_PAWN_GENE_H
#define PASSED_PAWN_GENE_H

#include "Gene.h"

#include <memory>
#include <string>

class Board;

class Passed_Pawn_Gene : public Gene
{
    public:
        double score_board(const Board& board) const override;

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;
};

#endif // PASSED_PAWN_GENE_H
