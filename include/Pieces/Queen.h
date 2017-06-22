#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

#include "Game/Color.h"

class Queen : public Piece
{
    public:
        explicit Queen(Color color);

        bool is_queen() const override;
};

#endif // QUEEN_H
