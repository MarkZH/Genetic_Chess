#ifndef PAWN_PROMOTION_BY_CAPTURE_H
#define PAWN_PROMOTION_BY_CAPTURE_H

#include "Pawn_Promotion.h"

#include <memory>

class Pawn_Promotion_by_Capture : public Pawn_Promotion
{
    public:
        Pawn_Promotion_by_Capture(std::shared_ptr<const Piece> promotion, char dir);
        Pawn_Promotion_by_Capture(const Pawn_Promotion_by_Capture&) = delete;
        Pawn_Promotion_by_Capture& operator=(const Pawn_Promotion_by_Capture&) = delete;
        ~Pawn_Promotion_by_Capture() override;

        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;

        std::string name() const override;
        std::string game_record_item(const Board& board, char file_start, int rank_start) const override;
};

#endif // PAWN_PROMOTION_BY_CAPTURE_H
