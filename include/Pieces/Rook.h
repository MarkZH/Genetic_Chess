#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece
{
    public:
        explicit Rook(Color color_in);
        Rook(const Rook&) = delete;
        Rook& operator=(const Rook&) = delete;

        bool is_rook() const override;
};

#endif // ROOK_H
