#include "Moves/Pawn_Move.h"

#include "Moves/Move.h"
#include "Game/Board.h"

Pawn_Move::Pawn_Move(Color color_in) : Move(0, (color_in == WHITE ? 1 : -1))
{
}

bool Pawn_Move::can_capture() const
{
    return false;
}

void Pawn_Move::side_effects(Board& board, char /* file_start */, int /* rank_start */) const
{
    board.repeat_count.clear();
}

std::string Pawn_Move::name() const
{
    return "Pawn Move";
}
