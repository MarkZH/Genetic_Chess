#include "Moves/Pawn_Double_Move.h"

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"

//! Create a pawn double move.

//! \param color The color of the moving pawn.
//! \param file_start The file of the starting square.
Pawn_Double_Move::Pawn_Double_Move(Color color, char file_start) :
    Pawn_Move(color, file_start, color == WHITE ? 2 : 7)
{
    adjust_end_rank(color == WHITE ? 1 : -1);
    if( ! end().inside_board())
    {
        throw std::invalid_argument(std::string("Invalid pawn double move starting at ") + start().string());
    }
}

//! In addition to normal pawn move side effects, the pawn double move makes the square the pawn jumps over targetable by an en passant capture.

//! \param board The square on which the double move is taking place.
void Pawn_Double_Move::side_effects(Board& board) const
{
    board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
    Pawn_Move::side_effects(board);
}
