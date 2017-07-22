#ifndef QUEENSIDE_CASTLE_H
#define QUEENSIDE_CASTLE_H

#include "Move.h"

#include <string>

class Board;

class Queenside_Castle : public Move
{
    public:
        Queenside_Castle(int base_rank);

        void side_effects(Board& board) const override;
        bool move_specific_legal(const Board& board) const override;
        bool can_capture() const override;
        std::string move_name() const override;
        std::string game_record_move_item(const Board&) const override;
};

#endif // QUEENSIDE_CASTLE_H
