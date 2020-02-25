#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <map>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Random.h"

class Board;

Piece_Strength_Gene::Piece_Strength_Gene() noexcept
{
    piece_strength.fill(100.0);
    recalculate_normalizing_value();
}

std::map<std::string, double> Piece_Strength_Gene::list_properties() const noexcept
{
    std::map<std::string, double> properties;
    for(auto piece_type : {Piece_Type::PAWN, Piece_Type::ROOK, Piece_Type::KNIGHT, Piece_Type::BISHOP, Piece_Type::QUEEN, Piece_Type::KING})
    {
        auto piece = Piece{Piece_Color::WHITE, piece_type};
        properties[std::string(1, piece.fen_symbol())] = piece_value(piece);
    }
    return properties;
}

void Piece_Strength_Gene::load_properties(const std::map<std::string, double>& properties)
{
    for(auto piece_type : {Piece_Type::PAWN, Piece_Type::ROOK, Piece_Type::KNIGHT, Piece_Type::BISHOP, Piece_Type::QUEEN, Piece_Type::KING})
    {
        auto piece = Piece{Piece_Color::WHITE, piece_type};
        piece_value(piece_type) = properties.at(std::string(1, piece.fen_symbol()));
    }

    recalculate_normalizing_value();
}

void Piece_Strength_Gene::gene_specific_mutation() noexcept
{
    auto& value = Random::random_element(piece_strength);
    value += Random::random_laplace(1.0 + std::sqrt(std::abs(value)));
    recalculate_normalizing_value();
}

void Piece_Strength_Gene::recalculate_normalizing_value() noexcept
{
    normalizing_value = 8*std::abs(piece_value(Piece_Type::PAWN)) +
                        2*std::abs(piece_value(Piece_Type::ROOK)) +
                        2*std::abs(piece_value(Piece_Type::KNIGHT)) +
                        2*std::abs(piece_value(Piece_Type::BISHOP)) +
                        1*std::abs(piece_value(Piece_Type::QUEEN)) +
                        1*std::abs(piece_value(Piece_Type::KING));
}

double Piece_Strength_Gene::piece_value(Piece_Type type) const noexcept
{
    return piece_strength[static_cast<unsigned>(type)];
}

double& Piece_Strength_Gene::piece_value(Piece_Type type) noexcept
{
    return piece_strength[static_cast<unsigned>(type)];
}

double Piece_Strength_Gene::piece_value(Piece piece) const noexcept
{
    if( ! piece)
    {
        return 0.0;
    }
    else
    {
        return piece_value(piece.type());
    }
}

const std::array<double, 6>& Piece_Strength_Gene::piece_values() const noexcept
{
    return piece_strength;
}

double Piece_Strength_Gene::normalizer() const noexcept
{
    return normalizing_value;
}

std::string Piece_Strength_Gene::name() const noexcept
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}
