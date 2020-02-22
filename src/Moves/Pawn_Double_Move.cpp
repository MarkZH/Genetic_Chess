#include "Moves/Pawn_Double_Move.h"

#include <cassert>

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Double_Move::Pawn_Double_Move(Piece_Color color, char file_start) noexcept :
    Pawn_Move(color, Square{file_start, color == Piece_Color::WHITE ? 2 : 7})
{
    adjust_end_rank(color == Piece_Color::WHITE ? 1 : -1);
    assert(end().inside_board());
}

void Pawn_Double_Move::side_effects(Board& board) const noexcept
{
    board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
}
