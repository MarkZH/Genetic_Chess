#include "Moves/En_Passant.h"

#include "Moves/Direction.h"
#include "Moves/Pawn_Capture.h"
#include "Game/Board.h"

En_Passant::En_Passant(Color color, Direction dir, char file_start, int rank_start) :
    Pawn_Capture(color, dir, file_start, rank_start)
{
    able_to_capture = false; // does not capture on destination square
    is_en_passant_move = true;
}

bool En_Passant::move_specific_legal(const Board& board) const
{
    return board.is_en_passant_targetable(ending_file, ending_rank);
}

void En_Passant::side_effects(Board& board) const
{
    board.remove_piece(ending_file, starting_rank);
    Pawn_Move::side_effects(board);
}
