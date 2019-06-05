#include "Moves/En_Passant.h"

#include <string>

#include "Moves/Direction.h"
#include "Moves/Pawn_Capture.h"
#include "Game/Board.h"

//! Create an en passant move.
//
//! \param color The color of the moving pawn.
//! \param dir The direction of the capture.
//! \param file_start The file of the square where the pawn starts.
En_Passant::En_Passant(Color color, Direction dir, char file_start) :
    Pawn_Capture(color, dir, file_start, color == WHITE ? 5 : 4)
{
    is_en_passant_move = true;
}

//! The pawn must end up on the square that was skipped by an immediately preceding Pawn_Double_Move.
//
//! \param board The board state just prior to the move.
//! \returns Whether the end square is a valid en passant target.
bool En_Passant::move_specific_legal(const Board& board) const
{
    return board.is_en_passant_targetable(end());
}

//! Implement capturing the pawn on the square to the side of the starting square.
//
//! \param board The board on which the capture is occurring.
void En_Passant::side_effects(Board& board) const
{
    board.remove_piece({end().file(), start().rank()});
    Pawn_Capture::side_effects(board);
}
