#ifndef EN_PASSANT_H
#define EN_PASSANT_H

#include "Pawn_Capture.h"

#include <string>

class Board;

class En_Passant : public Pawn_Capture
{
    public:
        explicit En_Passant(Color color, char dir);
        En_Passant(const En_Passant&) = delete;
        En_Passant& operator=(const En_Passant&) = delete;
        virtual ~En_Passant() override;

        void side_effects(Board& board, char file_start, int rank_start) const;
        bool is_legal(const Board& board, char file_start, int rank_start) const;
        std::string name() const;
};

#endif // EN_PASSANT_H
