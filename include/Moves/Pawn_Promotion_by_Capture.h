#ifndef PAWN_PROMOTION_BY_CAPTURE_H
#define PAWN_PROMOTION_BY_CAPTURE_H

#include <string>

#include "Pawn_Promotion.h"
#include "Direction.h"

class Pawn_Promotion_by_Capture : public Pawn_Promotion
{
    public:
        Pawn_Promotion_by_Capture(const Piece* promotion,
                                  Direction dir,
                                  char file_start);

        bool move_specific_legal(const Board& board) const override;

        std::string game_record_move_item(const Board& board) const override;
};

#endif // PAWN_PROMOTION_BY_CAPTURE_H
