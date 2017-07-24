#ifndef EN_PASSANT_H
#define EN_PASSANT_H

#include "Pawn_Capture.h"

#include <string>

class Board;

class En_Passant : public Pawn_Capture
{
    public:
        En_Passant(Color color, Capture_Direction dir, char file_start, int rank_start);

        void side_effects(Board& board) const override;
        bool move_specific_legal(const Board& board) const override;
        bool is_en_passant() const override;
};

#endif // EN_PASSANT_H
