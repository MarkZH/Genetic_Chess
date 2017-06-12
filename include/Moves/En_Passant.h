#ifndef EN_PASSANT_H
#define EN_PASSANT_H

#include "Pawn_Capture.h"

#include <string>

class Board;

class En_Passant : public Pawn_Capture
{
    public:
        En_Passant(Color color, char dir);

        void side_effects(Board& board, char file_start, int rank_start) const override;
        bool move_specific_legal(const Board& board, char file_start, int rank_start) const override;
        std::string name() const override;
};

#endif // EN_PASSANT_H
