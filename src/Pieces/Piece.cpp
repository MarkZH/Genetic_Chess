#include "Pieces/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>

#include "Game/Board.h"
#include "Utility.h"

Piece::Piece(Color color_in, const std::string& symbol_in, Piece_Type type_in) :
    my_color(color_in),
    symbol(symbol_in),
    my_type(type_in),
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

Piece_Type Piece::type() const
{
    return my_type;
}

void Piece::add_standard_legal_move(int file_step, int rank_step)
{
    for(char start_file = 'a'; start_file <= 'h'; ++start_file)
    {
        for(int start_rank = 1; start_rank <= 8; ++start_rank)
        {
            char end_file = start_file + file_step;
            int  end_rank = start_rank + rank_step;

            add_legal_move(std::make_unique<Move>(start_file, start_rank,
                                                  end_file, end_rank));
        }
    }
}

void Piece::add_legal_move(std::unique_ptr<Move> move)
{
    assert(Board::inside_board(move->start_file(), move->start_rank()));
    if(Board::inside_board(move->end_file(), move->end_rank()))
    {
        legal_moves[Board::board_index(move->start_file(), move->start_rank())].push_back(move.get());
        possible_moves.push_back(std::move(move));
    }
}

std::vector<Square> Piece::all_attacked_squares(char file, int rank, const Board& board) const
{
    std::vector<Square> result{};
    result.reserve(64);
    std::array<int, 2> blocking_direction{};

    for(const auto& move : get_move_list(file, rank))
    {
        if( ! move->can_capture())
        {
            continue;
        }

        if(blocking_direction == std::array<int, 2>{move->file_change(), move->rank_change()})
        {
            continue;
        }

        auto attacked_piece = board.piece_on_square(move->end_file(), move->end_rank());
        if(attacked_piece)
        {
            blocking_direction = {Math::sign(move->file_change()), Math::sign(move->rank_change())};
            if(attacked_piece->color() == color())
            {
                continue;
            }
        }

        result.push_back({move->end_file(), move->end_rank()});
    }

    return result;
}
