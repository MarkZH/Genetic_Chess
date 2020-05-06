#include "Genes/Total_Force_Gene.h"

#include <string>
#include <numeric>
#include <vector>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

#include "Utility/Random.h"

Total_Force_Gene::Total_Force_Gene(const Piece_Strength_Gene* piece_strength_source_in) noexcept : piece_strength_source(piece_strength_source_in)
{
}

double Total_Force_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    auto board_score = std::accumulate(Square::all_squares().begin(), Square::all_squares().end(), 0.0,
                                       [this, &board, perspective](auto sum, auto square)
                                       {
                                           auto piece = board.piece_on_square(square);
                                           if(piece && piece.color() == perspective)
                                           {
                                               return sum + piece_strength_source->piece_value(piece);
                                           }
                                           else
                                           {
                                               return sum;
                                           }
                                       });

    const auto& gated_pieces = board.gate_row(perspective);
    const auto base_rank = perspective == Piece_Color::WHITE ? 1 : 8;
    auto gate_score = 0.0;
    for(auto file = 'a'; file <= 'h'; ++file)
    {
        if( ! board.piece_has_moved({file, base_rank}))
        {
            gate_score += piece_strength_source->piece_value(gated_pieces[file - 'a']);
        }
    }

    return board_score + gate_factor*gate_score;
}

std::string Total_Force_Gene::name() const noexcept
{
    return "Total Force Gene";
}

void Total_Force_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept
{
    piece_strength_source = psg;
}

void Total_Force_Gene::gene_specific_mutation(const std::vector<Piece_Type>&) noexcept
{
    gate_factor += Random::random_laplace(0.03);
}

void Total_Force_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties["Gate Factor"] = gate_factor;
}

void Total_Force_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    gate_factor = properties.at("Gate Factor");
}
