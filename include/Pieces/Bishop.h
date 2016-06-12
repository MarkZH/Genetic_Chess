#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

enum Color;

class Bishop : public Piece
{
    public:
        explicit Bishop(Color color);
        Bishop(const Bishop&) = delete;
        Bishop& operator=(const Bishop&) = delete;
        virtual ~Bishop() override;
};

#endif // BISHOP_H
