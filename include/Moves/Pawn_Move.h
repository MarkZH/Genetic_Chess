#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Direction.h"
#include "Game/Color.h"
#include "Game/Square.h"

class Board;

//! \brief The simplest pawn move: one step forward.
class Pawn_Move : public Move
{
    public:
        //! \brief Creates a standard non-capturing pawn move.
        //!
        //! \param color_in The color of the pawn.
        //! \param start The square where the pawn starts.
        //! \param move_size How many squares the pawn moves (1 for normal move, 2 for double move from starting rank).
        //!
        //! The destination square of a pawn move is determined by the starting square and the color of the pawn.
        Pawn_Move(Piece_Color color_in, Square start, size_t move_size) noexcept;

        //! \brief Create a capturing pawn move.
        //!
        //! \param color_in The color of the moving pawn.
        //! \param start The square on which the move starts.
        //! \param file_change The direction (LEFT or RIGHT) of the capture.
        Pawn_Move(Piece_Color color_in, Square start, Direction file_change) noexcept;

        bool is_en_passant(const Board& board) const noexcept override;
        void side_effects(Board& board) const noexcept override;

    protected:
        //! Generic pawn move constructor to be called by other constuctors.
        //!
        //! \param start Square where the pawn move starts.
        //! \param end Square where the pawn move ends.
        Pawn_Move(Square start, Square end) noexcept;

    private:
        bool move_specific_legal(const Board& board) const noexcept override;
};

#endif // PAWN_MOVE_H
