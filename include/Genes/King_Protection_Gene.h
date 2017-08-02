#ifndef KING_PROTECTION_GENE_H
#define KING_PROTECTION_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>

class Board;

// Counts lines of attack to king
class King_Protection_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board) const override;
};

#endif // KING_PROTECTION_GENE_H
