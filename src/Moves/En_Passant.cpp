#include "Moves/En_Passant.h"
#include "Moves/Pawn_Capture.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"

En_Passant::En_Passant(Color color, char dir) : Pawn_Capture(color, dir)
{
}

En_Passant::~En_Passant()
{
}

bool En_Passant::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int rank_end = rank_start + rank_change();
    auto attacking_piece = board.view_piece_on_square(file_start, rank_start);
    auto attacked_piece  = board.view_piece_on_square(file_end,   rank_start);
    return board.is_en_passant_targetable(file_end, rank_end)
           && attacked_piece
           && attacking_piece->color() != attacked_piece->color();
}

void En_Passant::side_effects(Board& board, char file_start, int rank_start) const
{
    board.remove_piece(file_start + file_change(), rank_start);
    Pawn_Move::side_effects(board, file_start, rank_start);
}

std::string En_Passant::name() const
{
    return "En Passant";
}
