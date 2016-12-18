#ifndef KINGSIDE_CASTLE_H
#define KINGSIDE_CASTLE_H

#include "Move.h"

#include <string>

class Board;

class Kingside_Castle : public Move
{
    public:
        Kingside_Castle();
        Kingside_Castle(const Kingside_Castle&) = delete;
        Kingside_Castle& operator=(const Kingside_Castle&) = delete;
        ~Kingside_Castle() override;

        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;
        void side_effects(Board& board, char file_start, int rank_start) const override;
        std::string name() const override;
        std::string game_record_item(const Board&, char, int) const override;
};

#endif // KINGSIDE_CASTLE_H
