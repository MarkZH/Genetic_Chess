#include "Pieces/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>

#include "Game/Board.h"

Piece::Piece(Color color_in, const std::string& symbol_in) :
    my_color(color_in),
    symbol(symbol_in),
    legal_moves(64)
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
    return std::find_if(possible_moves.begin(),
                        possible_moves.end(),
                        [move](const auto& x){ return x.get() == move; }) != possible_moves.end();
}

const std::vector<Complete_Move>& Piece::get_move_list(char file, int rank) const
{
    return legal_moves.at(Board::board_index(file, rank));
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

void Piece::add_legal_move(const Move* move, char file, int rank)
{
    if(Board::inside_board(file + move->file_change(), rank + move->rank_change()))
    {
        legal_moves[Board::board_index(file, rank)].emplace_back(move, file, rank);
    }
}
