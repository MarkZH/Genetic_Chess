#include "Pieces/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>

#include "Game/Board.h"
#include "Moves/Move.h"


Piece::Piece(Color color_in, const std::string& symbol_in, Piece_Type type_in) :
    my_color(color_in),
    symbol(symbol_in),
    my_type(type_in)
{
}

Piece::~Piece()
{
}

Color Piece::color() const
{
    return my_color;
}

std::string Piece::pgn_symbol() const
{
    return type() == PAWN ? std::string{} : symbol;
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

const std::vector<const Move*>& Piece::move_list(char file, int rank) const
{
    return legal_moves[Board::square_index(file, rank)];
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
        legal_moves[Board::square_index(move->start_file(), move->start_rank())].push_back(move.get());
        possible_moves.push_back(std::move(move));
    }
}
