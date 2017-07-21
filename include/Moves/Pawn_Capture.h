#ifndef PAWN_CAPTURE_H
#define PAWN_CAPTURE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

enum Capture_Direction { LEFT, RIGHT };

class Pawn_Capture : public Pawn_Move
{
    public:
        Pawn_Capture(Color C, Capture_Direction dir);

        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        bool can_capture() const override;

        std::string name() const override;
        std::string game_record_item(const Board&, char file_start, int rank_start) const override;
};

#endif // PAWN_CAPTURE_H
