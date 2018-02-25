#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

#include "Game/Color.h"

class Queen : public Piece
{
    public:
        explicit Queen(Color color);
};

#endif // QUEEN_H
