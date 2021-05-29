#include "Genes/Total_Force_Gene.h"

#include <string>
#include <numeric>
#include <cmath>
#include <algorithm>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* const piece_strength_source_in) noexcept : piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
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

std::string Total_Force_Gene::name() const noexcept
{
    return "Total Force Gene";
}

void Total_Force_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* const psg) noexcept
{
    piece_strength_source = psg;
}

double Total_Force_Gene::game_progress(const Board& board) const noexcept
{
    const auto king_value = std::abs(piece_strength_source->piece_value({Piece_Color::WHITE, Piece_Type::KING}));
    std::array<double, 2> piece_value_left{};
    for(auto square : Square::all_squares())
    {
        if(const auto piece = board.piece_on_square(square))
        {
            piece_value_left[static_cast<size_t>(piece.color())] += std::abs(piece_strength_source->piece_value(piece));
        }
    }

    return (1.0 - *std::min_element(piece_value_left.begin(), piece_value_left.end()))/(1.0 - king_value);
}
