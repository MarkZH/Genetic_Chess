#ifndef EN_PASSANT_H
#define EN_PASSANT_H

#include "Pawn_Capture.h"
#include "Direction.h"

class Board;

//! En Passant: the weirdest and most obscure move in chess.
//
//! This is the only move where the square that the moving piece lands on
//! is different from the square the captured piece occupies.
//!
//! Really annoying to program; hence the is_en_passant() method.
class En_Passant : public Pawn_Capture
{
    public:
        En_Passant(Color color, Direction dir, char file_start);

        void side_effects(Board& board) const override;

    private:
        bool move_specific_legal(const Board& board) const override;
};

#endif // EN_PASSANT_H
