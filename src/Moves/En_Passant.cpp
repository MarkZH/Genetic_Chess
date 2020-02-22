#include "Moves/En_Passant.h"

#include <string>

#include "Moves/Direction.h"
#include "Moves/Pawn_Capture.h"
#include "Game/Board.h"

En_Passant::En_Passant(Piece_Color color, Direction dir, char file_start) noexcept :
    Pawn_Capture(color, dir, Square{file_start, color == Piece_Color::WHITE ? 5 : 4})
{
    mark_as_en_passant();
}

bool En_Passant::move_specific_legal(const Board& board) const noexcept
{
    return board.is_en_passant_targetable(end());
}

void En_Passant::side_effects(Board& board) const noexcept
{
    board.remove_piece({end().file(), start().rank()});
}
