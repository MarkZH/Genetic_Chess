#ifndef FREEDOM_TO_MOVE_GENE_H
#define FREEDOM_TO_MOVE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

#include "Game/Color.h"

class Board;

//! Scores board based on the number of attacking moves available.

//! The scoring of this gene counts empty squares and squares occupied
//! by opposing pieces. It does not take into account check or pins.
class Freedom_To_Move_Gene : public Gene
{
    public:
        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

        bool verify(const Board& board) const;

    private:
        double score_board(const Board& board, Color perspective, size_t depth) const override;

        size_t attack_count_scan(const Board& board, Color perspective) const;
};

#endif // FREEDOM_TO_MOVE_GENE_H
