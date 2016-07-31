#include "Pieces/Piece.h"
#include "Utility.h"
#include "Game/Board.h"
#include "Exceptions/Illegal_Move_Exception.h"

Piece::Piece(Color color_in) : my_color(color_in)
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

std::vector<std::shared_ptr<const Move>> Piece::get_legal_moves(const Board& board,
                                                                char file_start, int rank_start,
                                                                char file_end, int rank_end,
                                                                bool king_check,
                                                                char promote) const
{
    std::vector<std::shared_ptr<const Move>> legal_moves;
    if(board.whose_turn() != color())
    {
        return legal_moves;
    }

    for(const auto& move : possible_moves)
    {
        if(move->file_change() == (file_end - file_start) &&
           move->rank_change() == (rank_end - rank_start) &&
           board.is_legal(file_start, rank_start, move, king_check)
           && (( ! promote) || std::tolower(move->name().back()) == promote))
        {
            legal_moves.push_back(move);
        }
    }

    return legal_moves;
}

std::string Piece::pgn_symbol() const
{
    return symbol;
}

char Piece::fen_symbol() const
{
    return (my_color == WHITE ? std::toupper(symbol[0]) : std::tolower(symbol[0]));
}

bool Piece::can_move(const std::shared_ptr<const Move>& move) const
{
    for(const auto& possible_move : possible_moves)
    {
        if(possible_move == move)
        {
            return true;
        }
    }

    return false;
}

const std::vector<std::shared_ptr<const Move>> Piece::get_move_list() const
{
    return possible_moves;
}
