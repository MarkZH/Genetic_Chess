#ifndef EN_PASSANT_H
#define EN_PASSANT_H

#include "Pawn_Capture.h"
#include "Direction.h"

class Board;

//! \brief En Passant: the weirdest and most obscure move in chess.
//!
//! This is the only move where the square that the moving piece lands on
//! is different from the square the captured piece occupies.
//!
//! Really annoying to program; hence the is_en_passant() method.
class En_Passant : public Pawn_Capture
{
    public:
        //! \brief Create an en passant move.
        //!
        //! \param color The color of the moving pawn.
        //! \param dir The direction of the capture.
        //! \param file_start The file of the square where the pawn starts.
        En_Passant(Piece_Color color, Direction dir, char file_start) noexcept;

        //! \brief Implement capturing the pawn on the square to the side of the starting square.
        //!
        //! \param board The board on which the capture is occurring.
        void side_effects(Board& board) const noexcept override;

    private:
        //! \brief The pawn must end up on the square that was skipped by an immediately preceding Pawn_Double_Move.
        //!
        //! \param board The board state just prior to the move.
        //! \returns Whether the end square is a valid en passant target.
        bool move_specific_legal(const Board& board) const noexcept override;
};

#endif // EN_PASSANT_H
