#include "Moves/Pawn_Promotion.h"

#include <string>

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Piece.h"

//! Create a pawn promotion move.

//! \param promotion_piece The type of piece the pawn will be promoted to.
//! \param color The color of the moving pawn.
//! \param file_start The file of the starting square.
Pawn_Promotion::Pawn_Promotion(Piece_Type promotion_piece, Color color, char file_start) :
    Pawn_Move(color, file_start, color == WHITE ? 7 : 2),
    promote_to{color, promotion_piece}
{
}

//! In addition to normal pawn move side effects, the pawn gets replaced by another piece.

//! \param board The board on which the move is being made.
void Pawn_Promotion::side_effects(Board& board) const
{
    Pawn_Move::side_effects(board);
    board.place_piece(promote_to, end());
}

//! Attach an indication of the promotion piece to the normal pawn move record.

//! \param board The board state just before the move is made.
std::string Pawn_Promotion::game_record_move_item(const Board& board) const
{
    return Pawn_Move::game_record_move_item(board) + "=" + promotion_piece_symbol();
}

//! The symbol of the piece the pawn is promoted to.

//! \returns PGN symbol of the new piece.
char Pawn_Promotion::promotion_piece_symbol() const
{
    return promote_to.pgn_symbol().front();
}
