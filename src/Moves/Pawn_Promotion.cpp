#include "Moves/Pawn_Promotion.h"

#include <string>

#include "Moves/Pawn_Move.h"
#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Square.h"

Pawn_Promotion::Pawn_Promotion(Piece_Type promotion_piece, Piece_Color color, char file_start) noexcept :
    Pawn_Move(color, Square{file_start, color == Piece_Color::WHITE ? 7 : 2}),
    promote_to{color, promotion_piece}
{
}

void Pawn_Promotion::side_effects(Board& board) const noexcept
{
    board.place_piece(promote_to, end());
}

std::string Pawn_Promotion::algebraic_base(const Board& board) const noexcept
{
    return Pawn_Move::algebraic_base(board) + "=" + promotion_piece_symbol();
}

char Pawn_Promotion::promotion_piece_symbol() const noexcept
{
    return promote_to.pgn_symbol().front();
}
