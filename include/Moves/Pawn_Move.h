#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Game/Color.h"
#include "Game/Square.h"

//! \brief The simplest pawn move: one step forward.
class Pawn_Move : public Move
{
    public:
        //! \brief The destination square of a pawn move is determined by the starting square and the color of the pawn.
        //!
        //! \param color_in The color of the pawn.
        //! \param start The square where the pawn starts.
        //!
        //! The constructor also sets able_to_capture to false.
        Pawn_Move(Color color_in, Square start) noexcept;

    protected:
        std::string algebraic_base(const Board& board) const noexcept override;
};

#endif // PAWN_MOVE_H
