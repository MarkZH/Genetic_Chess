#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Direction.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

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
        //! \param promote If this move is a promotion move, this is the piece the pawn will be promoted to.
        //!
        //! The destination square of a pawn move is determined by the starting square and the color of the pawn.
        Pawn_Move(Piece_Color color_in, Square start, size_t move_size, Piece promote) noexcept;

        //! \brief Create a capturing pawn move.
        //!
        //! \param color_in The color of the moving pawn.
        //! \param start The square on which the move starts.
        //! \param file_change The direction (LEFT or RIGHT) of the capture.
        //! \param promote If this move is a promotion move, this is the piece the pawn will be promoted to.
        Pawn_Move(Piece_Color color_in, Square start, Direction file_change, Piece promote) noexcept;

        bool is_en_passant(const Board& board) const noexcept override;
        void side_effects(Board& board) const noexcept override;

        Piece promotion() const noexcept override;

        //! \brief The symbol of the piece the pawn is promoted to.
        //!
        //! \returns PGN symbol of the new piece.
        char promotion_piece_symbol() const noexcept override;


    protected:
        //! Generic pawn move constructor to be called by other constuctors.
        //!
        //! \param start Square where the pawn move starts.
        //! \param end Square where the pawn move ends.
        Pawn_Move(Square start, Square end) noexcept;

        //! \brief Attach an indication of the promotion piece to the normal pawn move record if needed.
        //!
        //! \param board The board state just before the move is made.
        std::string algebraic_base(const Board& board) const noexcept override;


    private:
        Piece promote_to;

        bool move_specific_legal(const Board& board) const noexcept override;
        void setup_promotion(Piece promote, Piece_Color pawn_color) noexcept;
};

#endif // PAWN_MOVE_H
