#include "Moves/Pawn_Double_Move.h"

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Double_Move::Pawn_Double_Move(const Piece_Color color, const char file_start) noexcept :
    Pawn_Move({file_start, color == Piece_Color::WHITE ? 2 : 7},
              {file_start, color == Piece_Color::WHITE ? 4 : 5})
{
}

void Pawn_Double_Move::side_effects(Board& board) const noexcept
{
    board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
}
