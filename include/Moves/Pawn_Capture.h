#ifndef PAWN_CAPTURE_H
#define PAWN_CAPTURE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

enum Capture_Direction { LEFT, RIGHT };

class Pawn_Capture : public Pawn_Move
{
    public:
        Pawn_Capture(Color color_in, Capture_Direction dir, char file_start, int rank_start);

        bool move_specific_legal(const Board& board) const override;
        bool can_capture() const override;

        std::string name() const override;
        std::string game_record_move_item(const Board&) const override;
};

#endif // PAWN_CAPTURE_H
