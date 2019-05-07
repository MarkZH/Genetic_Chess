#ifndef CASTLE_H
#define CASTLE_H

#include "Move.h"
#include "Direction.h"

#include <string>

class Board;

//! This class represents a castling move in either direction.
//
//! The only move that has two pieces change positions; hence the
//! special is_castling() method.
class Castle : public Move
{
    public:
        Castle(int base_rank, Direction direction);

        void side_effects(Board& board) const override;
        bool move_specific_legal(const Board& board) const override;

    protected:
        std::string game_record_move_item(const Board&) const override;

    private:
        Move rook_move;
};

#endif // CASTLE_H
