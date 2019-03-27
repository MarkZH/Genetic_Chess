#include "Moves/Pawn_Promotion.h"

#include <string>

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Piece.h"

Pawn_Promotion::Pawn_Promotion(const Piece* promotion_piece, char file_start) :
    Pawn_Move(promotion_piece->color(), file_start, promotion_piece->color() == WHITE ? 7 : 2),
    promote_to(promotion_piece)
{
}

void Pawn_Promotion::side_effects(Board& board) const
{
    Pawn_Move::side_effects(board);
    board.place_piece(promote_to, ending_file, ending_rank);
}

std::string Pawn_Promotion::game_record_move_item(const Board& board) const
{
    return Pawn_Move::game_record_move_item(board) + "=" + promotion_piece_symbol();
}

char Pawn_Promotion::promotion_piece_symbol() const
{
    return promote_to->pgn_symbol().front();
}
