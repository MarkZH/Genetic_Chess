#include "Moves/En_Passant.h"

#include "Moves/Move.h"
#include "Moves/Pawn_Capture.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"

En_Passant::En_Passant(Color color, Capture_Direction dir, char file_start, int rank_start) :
    Pawn_Capture(color, dir, file_start, rank_start)
{
}

bool En_Passant::move_specific_legal(const Board& board) const
{
    return board.is_en_passant_targetable(ending_file, ending_rank);
}

void En_Passant::side_effects(Board& board) const
{
    board.remove_piece(ending_file, starting_rank);
    Pawn_Move::side_effects(board);
}

std::string En_Passant::name() const
{
    return "En Passant";
}

bool En_Passant::is_en_passant() const
{
    return true;
}
