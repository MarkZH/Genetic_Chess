#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include "Gene.h"

#include <string>

#include "Game/Color.h"

class Board;

// Total number of legal moves
class Freedom_To_Move_Gene : public Gene
{
    public:
        Freedom_To_Move_Gene();

        Freedom_To_Move_Gene* duplicate() const override;

        std::string name() const override;

    private:
        size_t initial_number_of_moves;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // FREEDOM_TO_MOVE_GENE_H
