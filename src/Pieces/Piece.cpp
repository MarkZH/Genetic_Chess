#include "Pieces/Piece.h"

#include <cctype>

#include "Utility.h"
#include "Game/Board.h"
#include "Exceptions/Illegal_Move_Exception.h"

Piece::Piece(Color color_in, const std::string& symbol_in) :
    my_color(color_in),
    symbol(symbol_in)
{
}

Piece::~Piece()
{
}

std::string Piece::ascii_art(unsigned int row) const
{
    if(row >= ascii_art_lines.size())
    {
        return std::string(ascii_art_lines.front().size(), ' ');
    }
    return ascii_art_lines[row];
}

Color Piece::color() const
{
    return my_color;
}

bool Piece::operator==(const Piece& other) const
{
    return fen_symbol() == other.fen_symbol();
}

bool Piece::operator!=(const Piece& other) const
{
    return ! (*this == other);
}

std::string Piece::pgn_symbol() const
{
    return symbol;
}

char Piece::fen_symbol() const
{
    return (my_color == WHITE ? std::toupper(symbol[0]) : std::tolower(symbol[0]));
}

bool Piece::can_move(const Move* move) const
{
    for(const auto& possible_move : possible_moves)
    {
        if(possible_move.get() == move)
        {
            return true;
        }
    }

    return false;
}

const std::vector<std::unique_ptr<const Move>>& Piece::get_move_list() const
{
    return possible_moves;
}

bool Piece::is_pawn() const
{
    return false;
}

bool Piece::is_king() const
{
    return false;
}

bool Piece::is_queen() const
{
    return false;
}

bool Piece::is_rook() const
{
    return false;
}

bool Piece::is_bishop() const
{
    return false;
}

bool Piece::is_knight() const
{
    return false;
}
