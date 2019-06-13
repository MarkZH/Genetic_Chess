#include "Moves/En_Passant.h"

#include <string>

#include "Moves/Direction.h"
#include "Moves/Pawn_Capture.h"
#include "Game/Board.h"

En_Passant::En_Passant(Color color, Direction dir, char file_start) :
    Pawn_Capture(color, dir, file_start, color == WHITE ? 5 : 4)
{
    is_en_passant_move = true;
}

bool En_Passant::move_specific_legal(const Board& board) const
{
    return board.is_en_passant_targetable(end());
}

void En_Passant::side_effects(Board& board) const
{
    board.remove_piece({end().file(), start().rank()});
}
