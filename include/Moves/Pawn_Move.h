#ifndef PAWN_MOVE_H
#define PAWN_MOVE_H

#include "Move.h"
#include "Game/Color.h"

#include <string>

class Pawn_Move : public Move
{
    public:
        explicit Pawn_Move(Color color_in);
        Pawn_Move(const Pawn_Move&) = delete;
        Pawn_Move& operator=(const Pawn_Move&) = delete;

        void side_effects(Board& board, char file_start, int rank_end) const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_end) const override;
        bool can_capture() const override;
        std::string name() const override;
};

#endif // PAWN_MOVE_H
