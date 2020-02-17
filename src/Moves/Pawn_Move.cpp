#include "Moves/Pawn_Move.h"

#include <string>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Move::Pawn_Move(Color color_in, Square start_in) noexcept :
    Move(start_in,
         {start_in.file(), start_in.rank() + (color_in == WHITE ? 1 : -1)})
{
    disable_capturing();
}

std::string Pawn_Move::algebraic_base(const Board&) const noexcept
{
    return end().string();
}
