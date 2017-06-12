#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

enum Color;

class Bishop : public Piece
{
    public:
        explicit Bishop(Color color);

        bool is_bishop() const override;
};

#endif // BISHOP_H
