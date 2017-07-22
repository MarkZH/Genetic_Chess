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

const std::vector<const Move*>& Piece::get_move_list(char file, int rank) const
{
    return legal_moves[Board::board_index(file, rank)];
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

void Piece::add_standard_legal_move(int file_step, int rank_step)
{
    for(char start_file = 'a'; start_file <= 'h'; ++start_file)
    {
        for(int start_rank = 1; start_rank <= 8; ++start_rank)
        {
            char end_file = start_file + file_step;
            int  end_rank = start_rank + rank_step;

            if(Board::inside_board(end_file, end_rank))
            {
                auto move = std::make_unique<Move>(start_file, start_rank,
                                                   end_file, end_rank);
                add_legal_move(move.get());
                possible_moves.push_back(std::move(move));
            }
        }
    }
}

void Piece::add_special_legal_move(std::unique_ptr<Move> move)
{
    if(Board::inside_board(move->end_file(), move->end_rank()))
    {
        add_legal_move(move.get());
        possible_moves.push_back(std::move(move));
    }
}

void Piece::add_legal_move(const Move* move)
{
    legal_moves[Board::board_index(move->start_file(), move->start_rank())].push_back(move);
}
