#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

#include "Game/Color.h"

class Queen : public Piece
{
    public:
        explicit Queen(Color color);

        bool is_queen() const override;
        bool can_attack(int step_size, int file_step, int rank_step) const override;
};

#endif // QUEEN_H
