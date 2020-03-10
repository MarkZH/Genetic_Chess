#include "Moves/Pawn_Promotion_by_Capture.h"

#include <string>
#include <cassert>

#include "Game/Piece.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Direction.h"

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(Piece_Type promotion,
                                                     Piece_Color color,
                                                     Direction dir,
                                                     char file_start) noexcept :
    Pawn_Promotion(promotion, color, file_start)
{
    adjust_end_file(dir == Direction::RIGHT ? 1 : -1);
    assert(end().inside_board());
    enable_capturing();
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board) const noexcept
{
    return board.piece_on_square(end()); // must capture
}
