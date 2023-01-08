#include "Moves/Pawn_Promotion.h"

#include <string>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Square.h"

Pawn_Promotion::Pawn_Promotion(const Piece_Type promotion_piece, const Piece_Color color, const char file_start) noexcept :
    Pawn_Move(color, Square{file_start, color == Piece_Color::WHITE ? 7 : 2}),
    promote_to{color, promotion_piece}
{
}

Pawn_Promotion::Pawn_Promotion(const Piece_Type promotion_piece, const Piece_Color color, const char file_start, const Direction dir) noexcept :
    Pawn_Move(color, Square{file_start, color == Piece_Color::WHITE ? 7 : 2}, dir),
    promote_to{color, promotion_piece}
{
}

void Pawn_Promotion::side_effects(Board& board) const noexcept
{
    board.place_piece(promotion(), end());
}

Piece Pawn_Promotion::promotion() const noexcept
{
    return promote_to;
}

std::string Pawn_Promotion::algebraic_base(const Board& board) const noexcept
{
    return Pawn_Move::algebraic_base(board) + "=" + promotion_piece_symbol();
}

char Pawn_Promotion::promotion_piece_symbol() const noexcept
{
    return promotion().pgn_symbol().front();
}
