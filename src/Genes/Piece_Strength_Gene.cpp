#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <map>
#include <vector>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Random.h"

class Board;

Piece_Strength_Gene::Piece_Strength_Gene() noexcept
{
    piece_strength.fill(100.0);
    renormalize_values();
}

void Piece_Strength_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties.erase("Priority");

    const auto standard_all_pieces_score = 8*1.0 + // pawns
                                           2*5.0 + // rooks
                                           2*3.0 + // knights
                                           2*3.0 + // bishops
                                           1*9.0;  // queen
    auto standardize = (normalization() - std::abs(piece_value(Piece_Type::KING)))/standard_all_pieces_score;

    for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
    {
        auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
        properties[std::string(1, piece.fen_symbol())] = piece_value(piece.type())/standardize;
    }
}

void Piece_Strength_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    for(size_t piece_index = 0; piece_index < piece_strength.size(); ++piece_index)
    {
        auto piece = Piece{Piece_Color::WHITE, static_cast<Piece_Type>(piece_index)};
        piece_value(piece.type()) = properties.at(std::string(1, piece.fen_symbol()));
    }

    renormalize_values();
}

void Piece_Strength_Gene::gene_specific_mutation(const std::vector<Piece_Type>& gated_piece_types) noexcept
{
    const auto king_value = static_cast<int>(Piece_Type::KING);
    auto index = Random::random_integer(size_t{0}, king_value + gated_piece_types.size());
    auto type = index <= king_value ? static_cast<Piece_Type>(index) : gated_piece_types[index - king_value - 1];
    piece_value(type) += Random::random_laplace(0.003);
    renormalize_values();
}

void Piece_Strength_Gene::renormalize_values() noexcept
{
    auto normalizing_value = normalization();

    if(normalizing_value > 0.0)
    {
        for(auto& v : piece_strength)
        {
            v /= normalizing_value;
        }
    }
}

double Piece_Strength_Gene::normalization() const noexcept
{
    return 8*std::abs(piece_value(Piece_Type::PAWN)) +
           2*std::abs(piece_value(Piece_Type::ROOK)) +
           2*std::abs(piece_value(Piece_Type::KNIGHT)) +
           2*std::abs(piece_value(Piece_Type::BISHOP)) +
           1*std::abs(piece_value(Piece_Type::QUEEN)) +
           1*std::abs(piece_value(Piece_Type::KING));
}

double Piece_Strength_Gene::piece_value(Piece_Type type) const noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double& Piece_Strength_Gene::piece_value(Piece_Type type) noexcept
{
    return piece_strength[static_cast<int>(type)];
}

double Piece_Strength_Gene::piece_value(Piece piece) const noexcept
{
    if( ! piece || ! is_active())
    {
        return 0.0;
    }
    else
    {
        return piece_value(piece.type());
    }
}

const std::array<double, 16>& Piece_Strength_Gene::piece_values() const noexcept
{
    if(is_active())
    {
        return piece_strength;
    }
    else
    {
        static const auto inactive_piece_values = std::array<double, 16>{};
        return inactive_piece_values;
    }
}

size_t Piece_Strength_Gene::mutatable_components() const noexcept
{
    return 8;
}

std::string Piece_Strength_Gene::name() const noexcept
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}
