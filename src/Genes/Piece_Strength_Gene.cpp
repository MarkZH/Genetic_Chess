#include "Genes/Piece_Strength_Gene.h"

#include <cmath>
#include <limits>

#include "Utility.h"
#include "Pieces/Piece.h"

Piece_Strength_Gene::Piece_Strength_Gene()
{
    for(auto c : std::string("PRNBQK"))
    {
        piece_strength[c] = 0.0;
    }
    renormalize();
}

Piece_Strength_Gene::~Piece_Strength_Gene()
{
}

void Piece_Strength_Gene::reset_properties() const
{
    for(const auto& piece_score : piece_strength)
    {
        properties[std::string(1, piece_score.first)] = piece_score.second;
    }
}

void Piece_Strength_Gene::load_properties()
{
    for(const auto& piece_score : properties)
    {
        piece_strength[piece_score.first[0]] = piece_score.second;
    }
}

void Piece_Strength_Gene::mutate()
{
    Gene::mutate();

    for(auto& key_value : piece_strength)
    {
        const double mean_number_of_mutations = 2.0;
        if(Random::success_probability(mean_number_of_mutations/piece_strength.size()))
        {
            key_value.second += Random::random_normal(1.0);
        }
    }
    renormalize();
}

double Piece_Strength_Gene::piece_value(char symbol) const
{
    if(is_active())
    {
        return piece_strength.at(symbol)/normalizing_factor;
    }
    else
    {
        return 0.0;
    }
}

double Piece_Strength_Gene::piece_value(const std::shared_ptr<const Piece>& piece) const
{
    return piece_value(toupper(piece->fen_symbol()));
}

Piece_Strength_Gene* Piece_Strength_Gene::duplicate() const
{
    return new Piece_Strength_Gene(*this);
}

std::string Piece_Strength_Gene::name() const
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}

void Piece_Strength_Gene::renormalize()
{
    // Sum is equal to the total strength of a player's starting pieces
    // (8 pawns, 2 rooks, 2 knights, 2 bishops, 1 queen). The king is not
    // included because it is always on the board and would skew the Total
    // Force Gene. That gene subtract the value of the king from its
    // calculations.
    //
    // Use absolute value so there aren't discontinuous jumps in the
    // returned piece values when they suddenly switch sign.
    normalizing_factor = std::abs(8*piece_strength['P'] +
                                  2*piece_strength['R'] +
                                  2*piece_strength['N'] +
                                  2*piece_strength['B'] +
                                    piece_strength['Q']);

    if(normalizing_factor < std::numeric_limits<double>::epsilon())
    {
        normalizing_factor = 1.0;
    }
}
