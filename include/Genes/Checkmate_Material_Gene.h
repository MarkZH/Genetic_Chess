#ifndef CHECKMATE_MATERIAL_GENE_H
#define CHECKMATE_MATERIAL_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;

//! \brief Adds or subtracts points from the score based on if the player has enough pieces to checkmate their opponent.
class Checkmate_Material_Gene : public Clonable_Gene<Checkmate_Material_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
};

#endif // CHECKMATE_MATERIAL_GENE_H
