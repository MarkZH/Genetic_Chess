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
        virtual ~Queenside_Castle() override;

        void side_effects(Board& board, char file_start, int rank_start) const override;
        bool is_legal(const Board& board, char file_start, int rank_start, bool king_check) const override;
        virtual std::string name() const override;
        std::string game_record_item(const Board&, char, int) const override;
};

#endif // QUEENSIDE_CASTLE_H
