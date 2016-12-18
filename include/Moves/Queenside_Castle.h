#ifndef QUEENSIDE_CASTLE_H
#define QUEENSIDE_CASTLE_H

#include "Move.h"

#include <string>

class Board;

class Queenside_Castle : public Move
{
    public:
        Queenside_Castle();
        Queenside_Castle(const Queenside_Castle&) = delete;
        Queenside_Castle& operator=(const Queenside_Castle&) = delete;
        ~Queenside_Castle() override;

        void side_effects(Board& board, char file_start, int rank_start) const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;
        std::string name() const override;
        std::string game_record_item(const Board&, char, int) const override;
};

#endif // QUEENSIDE_CASTLE_H
