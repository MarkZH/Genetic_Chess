#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on the number of squares attacked by a side with bonus points for moves in enemy territory.
class Sphere_of_Influence_Gene : public Clonable_Gene<Sphere_of_Influence_Gene>
{
    public:
        Sphere_of_Influence_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, const size_t depth) const noexcept override;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
