#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

#include "Game/Color.h"

class Knight : public Piece
{
    public:
        explicit Knight(Color color);

        bool is_knight() const override;
        bool can_attack(int step_size, int file_step, int rank_step) const override;
};

#endif // KNIGHT_H
