#ifndef PAWN_DOUBLE_MOVE_H
#define PAWN_DOUBLE_MOVE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

class Board;

class Pawn_Double_Move : public Pawn_Move
{
    public:
        explicit Pawn_Double_Move(Color color);

        void side_effects(Board& board, char file_start, int rank_start) const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;
        std::string name() const override;
};

#endif // PAWN_DOUBLE_MOVE_H
