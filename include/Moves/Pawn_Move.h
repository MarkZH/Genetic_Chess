#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Game/Color.h"

//! The simplest pawn move: one step forward.
class Pawn_Move : public Move
{
    public:
        Pawn_Move(Color color_in, char file_start, int rank_start);

        void side_effects(Board& board) const override;

    protected:
        std::string game_record_move_item(const Board& board) const override;
};

#endif // PAWN_MOVE_H
