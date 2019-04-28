#include "Moves/Pawn_Move.h"

#include "Moves/Move.h"
#include "Game/Board.h"

//! The destination square of a pawn move is determined by the starting square and the color of the pawn.

//! \param color_in The color of the pawn.
//! \param file_start The file of the square where the pawn starts.
//! \param rank_start The rank of the square where the pawn starts.
//!
//! The constructor also sets able_to_capture to false.
Pawn_Move::Pawn_Move(Color color_in, char file_start, int rank_start) :
    Move({file_start, rank_start},
         {file_start, rank_start + (color_in == WHITE ? 1 : -1)})
{
    able_to_capture = false;
}

//! The side effect of all pawn moves is to reset the 50-move draw counter.

//! \param board The board on which the pawn move was made.
void Pawn_Move::side_effects(Board& board) const
{
    board.clear_repeat_count();
}

std::string Pawn_Move::game_record_move_item(const Board&) const
{
    return end().string();
}
