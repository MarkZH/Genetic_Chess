#include "Genes/Total_Force_Gene.h"

#include <numeric>
#include <cassert>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* const piece_strength_source_in) noexcept :
    Clonable_Gene("Total Force Gene"),
    piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    assert(piece_strength_source);
    const auto& values = piece_strength_source->piece_values();
    return std::accumulate(Square::all_squares().begin(), Square::all_squares().end(), 0.0,
                           [&values, &board, perspective](auto sum, auto square)
                           {
                               const auto piece = board.piece_on_square(square);
                               if(piece && piece.color() == perspective)
                               {
                                   return sum + values[static_cast<size_t>(piece.type())];
                               }
                               else
                               {
                                   return sum;
                               }
                           });
}

void Total_Force_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* const psg) noexcept
{
    piece_strength_source = psg;
}
