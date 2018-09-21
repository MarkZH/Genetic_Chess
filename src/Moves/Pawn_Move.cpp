#include "Moves/Pawn_Move.h"

#include "Moves/Move.h"
#include "Game/Board.h"

Pawn_Move::Pawn_Move(Color color_in, char file_start, int rank_start) :
    Move(file_start, rank_start,
         file_start, rank_start + (color_in == WHITE ? 1 : -1))
{
    able_to_capture = false;
}

void Pawn_Move::side_effects(Board& board) const
{
    board.clear_repeat_count();
}
