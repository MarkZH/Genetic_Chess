#ifndef KING_H
#define KING_H

#include "Piece.h"


class King : public Piece
{
    public:
        explicit King(Color color_in);
        King(const King&) = delete;
        King& operator=(const King&) = delete;

        bool is_king() const override;
};

#endif // KING_H
