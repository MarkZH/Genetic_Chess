#ifndef PAWN_PROMOTION_H
#define PAWN_PROMOTION_H

#include "Pawn_Move.h"

#include <string>

class Board;
class Piece;

//! When a pawn makes it to the far rank, it can promote to another non-king piece.
class Pawn_Promotion : public Pawn_Move
{
    public:
        Pawn_Promotion(const Piece* promotion_piece, char file_start);

        void side_effects(Board& board) const override;
        char promotion_piece_symbol() const override;

    protected:
        std::string game_record_move_item(const Board& board) const override;

    private:
        const Piece* promote_to;
};

#endif // PAWN_PROMOTION_H
