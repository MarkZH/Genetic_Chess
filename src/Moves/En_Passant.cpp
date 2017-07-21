#include "Moves/En_Passant.h"

#include "Moves/Move.h"
#include "Moves/Pawn_Capture.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"

En_Passant::En_Passant(Color color, Capture_Direction dir) : Pawn_Capture(color, dir)
{
}

bool En_Passant::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    return board.is_en_passant_targetable(file_start + file_change(),
                                          rank_start + rank_change());
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
