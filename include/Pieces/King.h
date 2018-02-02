#ifndef KING_H
#define KING_H

#include "Piece.h"

#include "Game/Color.h"

class King : public Piece
{
    public:
        explicit King(Color color_in);

        bool is_king() const override;
        bool can_attack(int step_size, int file_step, int rank_step) const override;
};

#endif // KING_H
