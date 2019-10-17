#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <memory>
#include <map>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Random.h"

class Board;

Piece_Strength_Gene::Piece_Strength_Gene()
{
    piece_strength.fill(100.0);
    recalculate_normalizing_value();
}

std::map<std::string, double> Piece_Strength_Gene::list_properties() const
{
    std::map<std::string, double> properties;
    for(auto piece_type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
    {
        auto piece = Piece{WHITE, piece_type};
        properties[std::string(1, piece.fen_symbol())] = piece_value(piece);
    }
    return properties;
}

void Piece_Strength_Gene::load_properties(const std::map<std::string, double>& properties)
{
    for(auto piece_type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
    {
        auto piece = Piece{WHITE, piece_type};
        piece_value(piece_type) = properties.at(std::string(1, piece.fen_symbol()));
    }

    recalculate_normalizing_value();
}

void Piece_Strength_Gene::gene_specific_mutation()
{
    Random::random_element(piece_strength) += Random::random_laplace(1.0);
    recalculate_normalizing_value();
}

void Piece_Strength_Gene::recalculate_normalizing_value()
{
    normalizing_value = 8*std::abs(piece_value(PAWN)) +
                        2*std::abs(piece_value(ROOK)) +
                        2*std::abs(piece_value(KNIGHT)) +
                        2*std::abs(piece_value(BISHOP)) +
                        1*std::abs(piece_value(QUEEN)) +
                        1*std::abs(piece_value(KING));
}

double Piece_Strength_Gene::piece_value(Piece_Type type) const
{
    return piece_strength[type];
}

double& Piece_Strength_Gene::piece_value(Piece_Type type)
{
    return piece_strength[type];
}

double Piece_Strength_Gene::piece_value(Piece piece) const
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

const std::array<double, 6>& Piece_Strength_Gene::piece_values() const
{
    return piece_strength;
}

double Piece_Strength_Gene::normalizer() const
{
    return normalizing_value;
}

std::unique_ptr<Gene> Piece_Strength_Gene::duplicate() const
{
    return std::make_unique<Piece_Strength_Gene>(*this);
}

std::string Piece_Strength_Gene::name() const
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Color, size_t) const
{
    return 0.0;
}
