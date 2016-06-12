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
        virtual ~Kingside_Castle() override;

        bool is_legal(const Board& board, char file_start, int rank_start) const;
        void side_effects(Board& board, char file_start, int rank_start) const;
        virtual std::string name() const;
        std::string game_record_item(const Board&, char, int) const;
};

#endif // KINGSIDE_CASTLE_H
