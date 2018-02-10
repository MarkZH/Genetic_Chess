#ifndef KING_H
#define KING_H

#include "Piece.h"

#include "Game/Color.h"

class King : public Piece
{
    public:
        explicit King(Color color_in);

        bool is_king() const override;
};

#endif // KING_H
