#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include "Gene.h"

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on the number of legal moves available.
class Freedom_To_Move_Gene : public Clonable_Gene<Freedom_To_Move_Gene>
{
    public:
        Freedom_To_Move_Gene() noexcept;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // FREEDOM_TO_MOVE_GENE_H
