#ifndef PAWN_PROMOTION_H
#define PAWN_PROMOTION_H

#include "Pawn_Move.h"

#include <string>

class Board;
class Piece;

class Pawn_Promotion : public Pawn_Move
{
    public:
        explicit Pawn_Promotion(const Piece* promotion);

        void side_effects(Board& board, char file_start, int rank_start) const override;
        std::string name() const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;
        std::string game_record_item(const Board& board, char file_start, int rank_start) const override;
        std::string coordinate_move(char file_start, int rank_start) const override;

    protected:
        const Piece* promote_to;
};

#endif // PAWN_PROMOTION_H
