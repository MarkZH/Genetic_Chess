#include "Moves/Pawn_Double_Move.h"

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Double_Move::Pawn_Double_Move(Color color, char file_start) :
    Pawn_Move(color, Square{file_start, color == WHITE ? 2 : 7})
{
    adjust_end_rank(color == WHITE ? 1 : -1);
    if( ! end().inside_board())
    {
        throw std::invalid_argument(std::string("Invalid pawn double move starting at ") + start().string());
    }
}

void Pawn_Double_Move::side_effects(Board& board) const
{
    board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
}
