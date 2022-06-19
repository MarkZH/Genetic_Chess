#ifndef CHECKMATE_MATERIAL_GENE_H
#define CHECKMATE_MATERIAL_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on if the player has enough pieces to checkmate their opponent.
class Checkmate_Material_Gene : public Clonable_Gene<Checkmate_Material_Gene>
{
    public:
        Checkmate_Material_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // CHECKMATE_MATERIAL_GENE_H
