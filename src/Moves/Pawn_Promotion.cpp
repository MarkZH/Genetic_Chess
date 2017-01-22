#include "Moves/Pawn_Promotion.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"

#include <memory>

Pawn_Promotion::Pawn_Promotion(const std::shared_ptr<const Piece>& promotion_piece) :
    Pawn_Move(promotion_piece->color()),
    promote_to(promotion_piece)
{
}

Pawn_Promotion::~Pawn_Promotion()
{
}

void Pawn_Promotion::side_effects(Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int  rank_end = rank_start + rank_change();

    Pawn_Move::side_effects(board, file_start, rank_start);
    board.place_piece(promote_to, file_end, rank_end); // will call dtor on this
}

std::string Pawn_Promotion::name() const
{
    return std::string("Pawn Promotion ") + promote_to->pgn_symbol();
}

bool Pawn_Promotion::move_specific_legal(const Board& /* board */, char /* file_start */, int rank_start) const
{
    return (rank_start == (rank_change() == 1 ? 7 : 2)); // promoting
}

bool Pawn_Promotion::can_capture() const
{
    return false;
}

std::string Pawn_Promotion::game_record_item(const Board& board, char file_start, int rank_start) const
{
    return Pawn_Move::game_record_item(board, file_start, rank_start) + "=" + promote_to->pgn_symbol();
}
