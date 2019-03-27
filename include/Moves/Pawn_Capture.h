#ifndef PAWN_CAPTURE_H
#define PAWN_CAPTURE_H

#include "Pawn_Move.h"
#include "Game/Color.h"
#include "Direction.h"

#include <string>

//! Pawn captures are special, so they get their own class.
class Pawn_Capture : public Pawn_Move
{
    public:
        Pawn_Capture(Color color_in, Direction dir, char file_start, int rank_start);

    protected:
        std::string game_record_move_item(const Board& board) const override;

    private:
        bool move_specific_legal(const Board& board) const override;
};

#endif // PAWN_CAPTURE_H
