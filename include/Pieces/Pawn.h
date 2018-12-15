#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

#include "Game/Color.h"

class Pawn : public Piece
{
    public:
        explicit Pawn(Color color_in);
};

#endif // PAWN_H
