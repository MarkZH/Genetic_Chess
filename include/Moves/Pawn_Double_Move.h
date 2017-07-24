#ifndef PAWN_DOUBLE_MOVE_H
#define PAWN_DOUBLE_MOVE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

class Board;

class Pawn_Double_Move : public Pawn_Move
{
    public:
        Pawn_Double_Move(Color color, char file_start);

        void side_effects(Board& board) const override;
};

#endif // PAWN_DOUBLE_MOVE_H
