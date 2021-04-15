#include "Moves/Pawn_Move.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in) noexcept :
    Pawn_Move(start_in,
              {start_in.file(), start_in.rank() + (color_in == Piece_Color::WHITE ? 1 : -1)})
{
}

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in, const Direction file_change) noexcept :
    Pawn_Move(start_in,
              {char(start_in.file() + (file_change == Direction::RIGHT ? 1 : -1)),
               start_in.rank() + (color_in == Piece_Color::WHITE ? 1 : -1)})
{
}

Pawn_Move::Pawn_Move(const Square start, const Square end) noexcept : Move(start, end)
{
    set_capturing_ability(start.file() != end.file());
}

bool Pawn_Move::move_specific_legal(const Board& board) const noexcept
{
    return bool(board.piece_on_square(end())) == can_capture();
}
