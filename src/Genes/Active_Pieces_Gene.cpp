#include "Genes/Active_Pieces_Gene.h"

#include <string>
#include <numeric>

#include "Game/Board.h"
#include "Game/Square.h"

std::string Active_Pieces_Gene::name() const noexcept
{
    return "Active Pieces Gene";
}

double Active_Pieces_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    auto squares = Square::all_squares();
    return std::accumulate(squares.begin(), squares.end(), 0,
                           [&board, perspective](auto so_far, auto square)
                           {
                               auto piece = board.piece_on_square(square);
                               return so_far + (piece &&
                                                piece.color() == perspective &&
                                                board.piece_has_moved(square));
                           })/16.0;
}
