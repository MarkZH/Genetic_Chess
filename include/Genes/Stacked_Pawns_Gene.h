#ifndef STACKED_PAWNS_GENE_H
#define STACKED_PAWNS_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;

//! \brief Scores a board based on the number of pawns with friendly pawns in front of them.
class Stacked_Pawns_Gene : public Clonable_Gene<Stacked_Pawns_Gene>
{
    public:
        std::string name() const noexcept override;

    private:
        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
};

#endif // STACKED_PAWNS_GENE_H
