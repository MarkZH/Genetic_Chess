#ifndef PAWN_PROMOTION_BY_CAPTURE_H
#define PAWN_PROMOTION_BY_CAPTURE_H

#include <string>

#include "Pawn_Promotion.h"

#include "Game/Piece_Types.h"
#include "Game/Color.h"
#include "Direction.h"

//! A pawn capture on the far rank that also results in a promotion.
class Pawn_Promotion_by_Capture : public Pawn_Promotion
{
    public:
        Pawn_Promotion_by_Capture(Piece_Type promotion,
                                  Color color,
                                  Direction dir,
                                  char file_start);

    protected:
        std::string game_record_move_item(const Board& board) const override;

    private:
        bool move_specific_legal(const Board& board) const override;
};

#endif // PAWN_PROMOTION_BY_CAPTURE_H
