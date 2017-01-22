#include "Moves/Pawn_Double_Move.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"


Pawn_Double_Move::Pawn_Double_Move(Color color) : Pawn_Move(color)
{
    d_rank = d_rank*2;
}

Pawn_Double_Move::~Pawn_Double_Move()
{
}

void Pawn_Double_Move::side_effects(Board& board, char file_start, int rank_start) const
{
    board.make_en_passant_targetable(file_start, rank_start + rank_change()/2);
    Pawn_Move::side_effects(board, file_start, rank_start);
}

bool Pawn_Double_Move::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    return ! board.piece_has_moved(file_start, rank_start);
}

bool Pawn_Double_Move::can_capture() const
{
    return false;
}

std::string Pawn_Double_Move::name() const
{
    return "Pawn Double Move";
}
