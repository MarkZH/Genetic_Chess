#ifndef PASSED_PAWN_GENE_H
#define PASSED_PAWN_GENE_H

#include "Gene.h"

#include <memory>
#include <string>

#include "Game/Color.h"

class Board;

//! Scores a board based on how many passed pawns (pawns with no opponent pawns ahead of them or in adjacent files) a player has.
class Passed_Pawn_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        //! Counts the number of passed pawns. A passed pawn is one with no opposing pawns on the same or adjacent files ahead of it.
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const override;
};

#endif // PASSED_PAWN_GENE_H
