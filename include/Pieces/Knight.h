#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

enum Color;

class Knight : public Piece
{
    public:
        explicit Knight(Color color);
        Knight(const Knight&) = delete;
        Knight& operator=(const Knight&) = delete;

        bool is_knight() const override;
};

#endif // KNIGHT_H
