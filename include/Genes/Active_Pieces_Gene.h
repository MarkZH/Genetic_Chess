#pragma once
#include "Genes/Gene.h"

//! \brief This gene counts the number of pieces that have been moved during the game.
class Active_Pieces_Gene : public Clonable_Gene<Active_Pieces_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

