#include "Moves/Pawn_Double_Move.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"


Pawn_Double_Move::Pawn_Double_Move(Color color) : Pawn_Move(color)
{
    d_rank = d_rank*2;
}

void Pawn_Double_Move::side_effects(Board& board, char file_start, int rank_start) const
{
    board.get_square(file_start, rank_start + rank_change()/2).make_en_passant_targetable();
    Pawn_Move::side_effects(board, file_start, rank_start);
}

bool Pawn_Double_Move::is_legal(const Board& board, char file_start, int rank_start) const
{
    auto color = board.view_square(file_start, rank_start).piece_on_square()->color();
    return rank_start == (color == WHITE ? 2 : 7)
            && board.view_square(file_start, rank_start + rank_change()/2).empty()
            && board.view_square(file_start, rank_start + rank_change()).empty();
}

std::string Pawn_Double_Move::name() const
{
    return "Pawn Double Move";
}
