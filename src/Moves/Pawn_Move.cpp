#include "Moves/Pawn_Move.h"

#include <cmath>

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in, const size_t move_size, const Piece promote) noexcept :
    Pawn_Move(start_in,
              start_in + Square_Difference{0, color_in == Piece_Color::WHITE ? int(move_size) : -int(move_size)})
{
    assert(move_size == 1 || move_size == 2);
    assert((move_size != 2) || (start_in.rank() == (color_in == Piece_Color::WHITE ? 2 : 7)));
    setup_promotion(promote, color_in);
}

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in, const Direction file_change, Piece promote) noexcept :
    Pawn_Move(start_in,
              start_in + Square_Difference{file_change == Direction::RIGHT ? 1 : -1,
                                           color_in == Piece_Color::WHITE ? 1 : -1})
{
    setup_promotion(promote, color_in);
}

bool Pawn_Move::is_en_passant(const Board& board) const noexcept
{
    return board.en_passant_target == end();
}

void Pawn_Move::side_effects(Board& board) const noexcept
{
    if(std::abs(rank_change()) == 2)
    {
        board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
    }
    else if(end().rank() == 1 || end().rank() == 8)
    {
        board.place_piece(promotion(), end());
    }
}

Pawn_Move::Pawn_Move(const Square start, const Square end) noexcept : Move(start, end)
{
    set_capturing_ability(start.file() != end.file());
}

bool Pawn_Move::move_specific_legal(const Board& board) const noexcept
{
    return (bool(board.piece_on_square(end())) == can_capture()) || is_en_passant(board);
}

Piece Pawn_Move::promotion() const noexcept
{
    return promote_to;
}

std::string Pawn_Move::algebraic_base(const Board& board) const noexcept
{
    return Move::algebraic_base(board) + (promotion() ? (std::string("=") + promotion_piece_symbol()) : "");
}

char Pawn_Move::promotion_piece_symbol() const noexcept
{
    return promotion() ? promotion().pgn_symbol().front() : '\0';
}

void Pawn_Move::setup_promotion(const Piece promote, [[maybe_unused]] const Piece_Color pawn_color) noexcept
{
    assert( ! promote || promote.color() == pawn_color);
    assert( ! promote || start().rank() == (pawn_color == Piece_Color::WHITE ? 7 : 2));
    pawn_color;
    promote_to = promote;
}