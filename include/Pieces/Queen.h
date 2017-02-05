#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

enum Color;

class Queen : public Piece
{
    public:
        explicit Queen(Color color);
        Queen(const Queen&) = delete;
        Queen& operator=(const Queen&) = delete;
        ~Queen() override;

        bool is_queen() const override;
};

#endif // QUEEN_H
