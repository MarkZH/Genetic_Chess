#ifndef CHECKMATE_MATERIAL_GENE_H
#define CHECKMATE_MATERIAL_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Adds or subtracts points from the score based on if the player has enough pieces to checkmate their opponent.
class Checkmate_Material_Gene : public Clonable_Gene<Checkmate_Material_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
};

#endif // CHECKMATE_MATERIAL_GENE_H
