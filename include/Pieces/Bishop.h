#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

#include "Game/Color.h"

class Bishop : public Piece
{
    public:
        explicit Bishop(Color color);

        bool is_bishop() const override;
        bool can_attack(int step_size, int file_step, int rank_step) const override;
};

#endif // BISHOP_H
