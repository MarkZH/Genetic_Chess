#ifndef PAWN_CAPTURE_H
#define PAWN_CAPTURE_H

#include "Pawn_Move.h"
#include "Game/Color.h"
#include "Direction.h"

#include <string>

//! \brief Pawn captures are special, so they get their own class.
class Pawn_Capture : public Pawn_Move
{
    public:
        //! \brief Pawn captures are identified by the color of the pawn, the direction of the capture, and the starting square.
        //!
        //! \param color_in The color of the capturing pawn.
        //! \param dir The direction of the move horizontally: LEFT or RIGHT.
        //! \param start The square where the move starts.
        Pawn_Capture(Piece_Color color_in, Direction dir, Square start);

    private:
        //! \brief This move must capture.
        //!
        //! \param board The board state just before this move is to be made.
        bool move_specific_legal(const Board& board) const noexcept override;
};

#endif // PAWN_CAPTURE_H
