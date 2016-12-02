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
        virtual ~Pawn_Move() override;

        virtual void side_effects(Board& board, char file_start, int rank_end) const override;
        virtual bool is_legal(const Board& board, char file_start, int rank_end, bool king_check) const override;
        virtual std::string name() const override;
        virtual std::string game_record_item(const Board& board, char file_start, int rank_start) const override;
};

#endif // PAWN_MOVE_H
