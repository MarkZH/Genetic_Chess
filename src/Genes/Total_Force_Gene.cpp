#include "Genes/Total_Force_Gene.h"

#include <string>
#include <numeric>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source_in) noexcept : piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, Piece_Color perspective, size_t, double) const noexcept
{
    auto values = piece_strength_source->piece_values();
    return std::accumulate(Square::all_squares().begin(), Square::all_squares().end(), 0.0,
                           [&values, &board, perspective](auto sum, auto square)
                           {
                               auto piece = board.piece_on_square(square);
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

std::string Total_Force_Gene::name() const noexcept
{
    return "Total Force Gene";
}

void Total_Force_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept
{
    piece_strength_source = psg;
}
