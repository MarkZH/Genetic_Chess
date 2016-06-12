#ifndef PAWN_H
#define PAWN_H

#include <string>

#include "Piece.h"


class Pawn : public Piece
{
    public:
        explicit Pawn(Color color_in);
        Pawn(const Pawn&) = delete;
        Pawn& operator=(const Pawn&) = delete;
        std::string pgn_symbol() const override;
        virtual ~Pawn() override;
};

#endif // PAWN_H
