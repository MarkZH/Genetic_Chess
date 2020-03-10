#ifndef PAWN_PROMOTION_H
#define PAWN_PROMOTION_H

#include "Pawn_Move.h"

#include <string>

#include "Direction.h"
#include "Game/Piece.h"
#include "Game/Color.h"

class Board;

//! \brief When a pawn makes it to the far rank, it can promote to another non-king piece.
class Pawn_Promotion : public Pawn_Move
{
    public:
        //! \brief Create a pawn promotion move.
        //!
        //! \param promotion_piece The type of piece the pawn will be promoted to.
        //! \param color The color of the moving pawn.
        //! \param file_start The file of the starting square.
        Pawn_Promotion(Piece_Type promotion_piece, Piece_Color color, char file_start) noexcept;

        //! \brief Create a capturing pawn promotion move.
        //!
        //! \param promotion_piece The type of piece the pawn will be promoted to.
        //! \param color The color of the moving pawn.
        //! \param file_start The file of the starting square.
        //! \param file_change The direction of the capture (LEFT or RIGHT).
        Pawn_Promotion(Piece_Type promotion_piece, Piece_Color color, char file_start, Direction file_change) noexcept;

        //! \brief Replace the pawn with the promoted piece.
        //!
        //! \param board The board on which the move is being made.
        void side_effects(Board& board) const noexcept override;

        //! \brief The symbol of the piece the pawn is promoted to.
        //!
        //! \returns PGN symbol of the new piece.
        char promotion_piece_symbol() const noexcept override;

    protected:
        //! \brief Attach an indication of the promotion piece to the normal pawn move record.
        //!
        //! \param board The board state just before the move is made.
        std::string algebraic_base(const Board& board) const noexcept override;

    private:
        const Piece promote_to;
};

#endif // PAWN_PROMOTION_H
