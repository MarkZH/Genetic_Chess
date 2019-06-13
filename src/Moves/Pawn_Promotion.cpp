#include "Moves/Pawn_Promotion.h"

#include <string>

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Piece.h"

Pawn_Promotion::Pawn_Promotion(Piece_Type promotion_piece, Color color, char file_start) :
    Pawn_Move(color, file_start, color == WHITE ? 7 : 2),
    promote_to{color, promotion_piece}
{
}

void Pawn_Promotion::side_effects(Board& board) const
{
    board.place_piece(promote_to, end());
}

std::string Pawn_Promotion::game_record_move_item(const Board& board) const
{
    return Pawn_Move::game_record_move_item(board) + "=" + promotion_piece_symbol();
}

char Pawn_Promotion::promotion_piece_symbol() const
{
    return promote_to.pgn_symbol().front();
}
