#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

#include "Game/Color.h"

class Bishop : public Piece
{
    public:
        explicit Bishop(Color color);
};

#endif // BISHOP_H
