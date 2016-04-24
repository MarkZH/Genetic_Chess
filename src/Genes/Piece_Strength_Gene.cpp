#include "Genes/Piece_Strength_Gene.h"

#include <cmath>

#include "Utility.h"
#include "Pieces/Piece.h"

Piece_Strength_Gene::Piece_Strength_Gene() : Gene(0.0)
{
    for(auto c : std::string("PRNBQK"))
    {
        piece_strength[c] = 1.0;
    }

    reset_properties();
}

void Piece_Strength_Gene::reset_properties()
{
    for(auto c : std::string("PRNBQK"))
    {
        properties[String::to_string(c)] = &piece_strength[c];
    }
}

Piece_Strength_Gene::~Piece_Strength_Gene()
{
}

void Piece_Strength_Gene::mutate()
{
    for(auto& key_value : piece_strength)
    {
        key_value.second = std::max(key_value.second + Random::random_normal(1.0), 0.0);
    }
}

double Piece_Strength_Gene::piece_value(char symbol) const
{
    return piece_strength.at(symbol);
}

double Piece_Strength_Gene::piece_value(const std::shared_ptr<const Piece>& piece) const
{
    return piece_value(toupper(piece->fen_symbol()));
}

Piece_Strength_Gene* Piece_Strength_Gene::duplicate() const
{
    auto dupe = new Piece_Strength_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string Piece_Strength_Gene::name() const
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}
