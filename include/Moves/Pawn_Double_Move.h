#ifndef PAWN_DOUBLE_MOVE_H
#define PAWN_DOUBLE_MOVE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

class Board;

class Pawn_Double_Move : public Pawn_Move
{
    public:
        explicit Pawn_Double_Move(Color color);
        Pawn_Double_Move(const Pawn_Double_Move&) = delete;
        Pawn_Double_Move& operator=(const Pawn_Double_Move&) = delete;
        virtual ~Pawn_Double_Move() override;

        void side_effects(Board& board, char file_start, int rank_start) const;
        bool is_legal(const Board& board, char file_start, int rank_start) const;
        std::string name() const;
};

#endif // PAWN_DOUBLE_MOVE_H
