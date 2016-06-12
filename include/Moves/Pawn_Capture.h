#ifndef PAWN_CAPTURE_H
#define PAWN_CAPTURE_H

#include "Pawn_Move.h"
#include "Game/Color.h"

#include <string>

class Pawn_Capture : public Pawn_Move
{
    public:
        explicit Pawn_Capture(Color C, char dir); // dir = 'l' (left) or 'r' (right)
        Pawn_Capture(const Pawn_Capture&) = delete;
        Pawn_Capture& operator=(const Pawn_Capture&) = delete;
        virtual ~Pawn_Capture() override;

        virtual bool is_legal(const Board& board, char file_start, int rank_start) const;
        virtual std::string name() const;
        virtual std::string game_record_item(const Board&, char file_start, int rank_start) const;
};

#endif // PAWN_CAPTURE_H
