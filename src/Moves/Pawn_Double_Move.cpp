#include "Moves/Pawn_Double_Move.h"

#include "Moves/Move.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"


Pawn_Double_Move::Pawn_Double_Move(Color color, char file_start) :
    Pawn_Move(color, file_start, color == WHITE ? 2 : 7)
{
    ending_rank += (color == WHITE ? 1 : -1);
}

void Pawn_Double_Move::side_effects(Board& board) const
{
    board.make_en_passant_targetable(starting_file, (starting_rank + ending_rank)/2);
    Pawn_Move::side_effects(board);
}

std::string Pawn_Double_Move::move_name() const
{
    return "Pawn Double Move";
}
