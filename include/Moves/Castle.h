#ifndef CASTLE_H
#define CASTLE_H

#include "Move.h"
#include "Direction.h"

#include <string>

class Board;

class Castle : public Move
{
    public:
        Castle(int base_rank, Direction direction);

        void side_effects(Board& board) const override;
        bool move_specific_legal(const Board& board) const override;
        std::string game_record_move_item(const Board&) const override;

    private:
        char rook_starting_file;
        char rook_ending_file;
};

#endif // CASTLE_H
