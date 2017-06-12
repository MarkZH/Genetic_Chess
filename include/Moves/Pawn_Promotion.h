#ifndef PAWN_PROMOTION_H
#define PAWN_PROMOTION_H

#include "Pawn_Move.h"

#include <string>
#include <memory>

class Board;
class Piece;

class Pawn_Promotion : public Pawn_Move
{
    public:
        explicit Pawn_Promotion(std::shared_ptr<const Piece> promotion);
        Pawn_Promotion(const Pawn_Promotion&) = delete;
        Pawn_Promotion& operator=(const Pawn_Promotion&) = delete;

        void side_effects(Board& board, char file_start, int rank_start) const override;
        std::string name() const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;
        std::string game_record_item(const Board& board, char file_start, int rank_start) const override;
        std::string coordinate_move(char file_start, int rank_start) const override;

    protected:
        std::shared_ptr<const Piece> promote_to;
};

#endif // PAWN_PROMOTION_H
