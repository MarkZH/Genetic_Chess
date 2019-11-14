#ifndef CASTLE_H
#define CASTLE_H

#include "Move.h"
#include "Direction.h"

#include <string>

class Board;

//! This class represents a castling move in either direction.
//
//! The only move that has two pieces change positions; hence the
//! special is_castling() method.
class Castle : public Move
{
    public:
        //! Construct a castling move in a certain direction.
        //
        //! \param base_rank The back rank of the player: 1 for white, 8 for black.
        //! \param direction The direction of the king's move: LEFT for queenside, RIGHT for king side.
        Castle(int base_rank, Direction direction);

        //! Moves the rook to its final square.
        //
        //! This overloaded method also records when the castling move was made.
        //! \param board The board on which the move is being made.
        void side_effects(Board& board) const override;

        //! Implements the rules for castling.
        //
        //! Namely:
        //! - The king and the rook towards which the king moves have not moved during the game.
        //! - The king is not in check.
        //! - All of the squares between the king and rook are vacant.
        //! - The king does not cross a square that is attacked by an opponent's piece.
        //! \param board The board on which castling is being considered.
        //! \returns Whether the current board position allows for castling.
        bool move_specific_legal(const Board& board) const override;

    protected:
        //! Castling moves have a special notation in PGN.
        //
        //! \returns "O-O" for kingside castling or "O-O-O" for queenside castling.
        std::string game_record_move_item(const Board&) const override;

    private:
        Move rook_move;
};

#endif // CASTLE_H
