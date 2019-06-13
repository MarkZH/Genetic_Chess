#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Game/Color.h"

//! The simplest pawn move: one step forward.
class Pawn_Move : public Move
{
    public:
        Pawn_Move(Color color_in, char file_start, int rank_start);
        //! The destination square of a pawn move is determined by the starting square and the color of the pawn.
        //
        //! \param color_in The color of the pawn.
        //! \param start The square where the pawn starts.
        //!
        //! The constructor also sets able_to_capture to false.

    protected:
        std::string game_record_move_item(const Board& board) const override;
};

#endif // PAWN_MOVE_H
