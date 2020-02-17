#include "Moves/Pawn_Capture.h"

#include <string>
#include <cassert>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Game/Piece.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Capture::Pawn_Capture(Color color_in, Direction dir, Square start_in) :
    Pawn_Move(color_in, start_in)
{
    adjust_end_file(dir == Direction::RIGHT ? 1 : -1);
    assert(end().inside_board());
    enable_capturing();
}

bool Pawn_Capture::move_specific_legal(const Board& board) const noexcept
{
    return board.piece_on_square(end()); // must capture a piece
}

std::string Pawn_Capture::algebraic_base(const Board& board) const noexcept
{
    return start().file() + std::string("x") + Pawn_Move::algebraic_base(board);
}
