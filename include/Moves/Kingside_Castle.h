#ifndef KINGSIDE_CASTLE_H
#define KINGSIDE_CASTLE_H

#include "Move.h"

#include <string>

class Board;

class Kingside_Castle : public Move
{
    public:
        explicit Kingside_Castle(int base_rank);

        bool move_specific_legal(const Board& board) const override;
        bool can_capture() const override;
        void side_effects(Board& board) const override;
        std::string name() const override;
        std::string game_record_move_item(const Board&) const override;
};

#endif // KINGSIDE_CASTLE_H
