#include "Moves/Pawn_Move.h"

#include <string>

#include "Moves/Move.h"
#include "Game/Board.h"

Pawn_Move::Pawn_Move(Color color_in, char file_start, int rank_start) :
    Move({file_start, rank_start},
         {file_start, rank_start + (color_in == WHITE ? 1 : -1)})
{
    able_to_capture = false;
}

std::string Pawn_Move::game_record_move_item(const Board&) const
{
    return end().string();
}
