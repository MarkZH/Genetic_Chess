#ifndef KING_PROTECTION_GENE_H
#define KING_PROTECTION_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Scores the board based on how many lines of attack to king there are.
class King_Protection_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective, size_t depth) const override;
};

#endif // KING_PROTECTION_GENE_H
