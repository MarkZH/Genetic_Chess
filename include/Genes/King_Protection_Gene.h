#ifndef KING_PROTECTION_GENE_H
#define KING_PROTECTION_GENE_H

#include "Genes/Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Scores the board based on how many lines of attack to king there are.
//
//! Counts the number of empty squares from which a piece could attack the king. This
//! is a measure of the exposure of the king
class King_Protection_Gene : public Clonable_Gene<King_Protection_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // KING_PROTECTION_GENE_H
