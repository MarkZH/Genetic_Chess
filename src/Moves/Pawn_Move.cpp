#include "Moves/Pawn_Move.h"

#include <cmath>

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in, const size_t move_size) noexcept :
    Pawn_Move(start_in,
              start_in + Square_Difference{0, color_in == Piece_Color::WHITE ? int(move_size) : -int(move_size)})
{
    assert(move_size == 1 || move_size == 2);
    assert((move_size != 2) || (start_in.rank() == (color_in == Piece_Color::WHITE ? 2 : 7)));
}

Pawn_Move::Pawn_Move(const Piece_Color color_in, const Square start_in, const Direction file_change) noexcept :
    Pawn_Move(start_in,
              start_in + Square_Difference{file_change == Direction::RIGHT ? 1 : -1,
                                           color_in == Piece_Color::WHITE ? 1 : -1})
{
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
}

Pawn_Move::Pawn_Move(const Square start, const Square end) noexcept : Move(start, end)
{
    set_capturing_ability(start.file() != end.file());
}

bool Pawn_Move::move_specific_legal(const Board& board) const noexcept
{
    return (bool(board.piece_on_square(end())) == can_capture()) || is_en_passant(board);
}

