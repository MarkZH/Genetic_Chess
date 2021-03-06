#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;

//! \brief Scores board based on the number of attacking moves available.
//!
//! The scoring of this gene counts empty squares and squares occupied
//! by opposing pieces. It does not take into account check or pins.
class Freedom_To_Move_Gene : public Clonable_Gene<Freedom_To_Move_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
};

#endif // FREEDOM_TO_MOVE_GENE_H
