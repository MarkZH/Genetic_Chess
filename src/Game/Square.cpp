#include "Game/Square.h"
#include "Pieces/Piece.h"

Square::Square() :
    piece_on_this_square(nullptr),
    is_en_passant_target(false),
    piece_moved(false)
{
}

std::shared_ptr<const Piece> Square::piece_on_square() const
{
    return piece_on_this_square;
}

std::shared_ptr<const Piece> Square::piece_on_square()
{
    return piece_on_this_square;
}

bool Square::is_en_passant_targetable() const
{
    return is_en_passant_target;
}

void Square::remove_en_passant_targetable()
{
    is_en_passant_target = false;
}

void Square::make_en_passant_targetable()
{
    is_en_passant_target = true;
}

void Square::place_piece(const std::shared_ptr<const Piece>& p)
{
    piece_on_this_square = p;
}

bool Square::empty() const
{
    return piece_on_square() == nullptr;
}

bool Square::piece_has_moved() const
{
    return piece_moved;
}

void Square::player_moved_piece()
{
    piece_moved = true;
}
