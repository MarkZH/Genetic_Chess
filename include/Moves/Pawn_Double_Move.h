#ifndef PAWN_DOUBLE_MOVE_H
#define PAWN_DOUBLE_MOVE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

class Board;

//! \brief A class representing a pawn double move.
class Pawn_Double_Move : public Pawn_Move
{
    public:
        //! \brief Create a pawn double move.
        //!
        //! \param color The color of the moving pawn.
        //! \param file_start The file of the starting square.
        Pawn_Double_Move(Piece_Color color, char file_start) noexcept;

        //! \brief In addition to normal pawn move side effects, the pawn double move makes the square the pawn jumps over targetable by an en passant capture.
        //!
        //! \param board The square on which the double move is taking place.
        void side_effects(Board& board) const noexcept override;
};

#endif // PAWN_DOUBLE_MOVE_H
