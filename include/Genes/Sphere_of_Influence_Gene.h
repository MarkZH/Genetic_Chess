#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Scores a board based on the number of squares attacked by a side with bonus points for legality and moves close to the king.
class Sphere_of_Influence_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    private:
        double score_board(const Board& board, Color perspective, const size_t prior_real_moves) const override;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
