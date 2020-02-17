#ifndef PAWN_PROMOTION_BY_CAPTURE_H
#define PAWN_PROMOTION_BY_CAPTURE_H

#include <string>

#include "Pawn_Promotion.h"

#include "Game/Piece.h"
#include "Game/Color.h"
#include "Direction.h"

//! A pawn capture on the far rank that also results in a promotion.
class Pawn_Promotion_by_Capture : public Pawn_Promotion
{
    public:
        //! Create a pawn capture that also promotes.
        //
        //! \param promotion A type of piece the pawn will be promoted to.
        //! \param color The color of the moving pawn.
        //! \param dir The direction of the capture.
        //! \param file_start The horizontal direction of the capture.
        Pawn_Promotion_by_Capture(Piece_Type promotion,
                                  Color color,
                                  Direction dir,
                                  char file_start) noexcept;

    protected:
        //! Combine pawn capture note with a promotion note.
        //
        //! \param board The board state just before the move.
        //! \returns A textual record of a capture and a promotion.
        std::string algebraic_base(const Board& board) const noexcept override;

    private:
        //! This move must capture.
        //
        //! \param board The board state just before the move.
        //! \returns Whether there is an opposing piece to capture.
        bool move_specific_legal(const Board& board) const noexcept override;
};

#endif // PAWN_PROMOTION_BY_CAPTURE_H
