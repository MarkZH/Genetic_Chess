#ifndef PAWN_PROMOTION_H
#define PAWN_PROMOTION_H

#include "Pawn_Move.h"

#include <string>

class Board;
class Piece;

class Pawn_Promotion : public Pawn_Move
{
    public:
        explicit Pawn_Promotion(const Piece* promotion_piece, char file_start);

        void side_effects(Board& board) const override;
        std::string move_name() const override;
        std::string game_record_move_item(const Board& board) const override;
        std::string coordinate_move() const override;

    protected:
        const Piece* promote_to;
};

#endif // PAWN_PROMOTION_H
