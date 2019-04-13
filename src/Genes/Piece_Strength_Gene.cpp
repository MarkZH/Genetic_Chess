#include "Genes/Piece_Strength_Gene.h"

#include <string>
#include <array>
#include <memory>
#include <map>

#include "Genes/Gene.h"
#include "Game/Piece.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility/Random.h"

//! Initialize the Piece values to zero.
Piece_Strength_Gene::Piece_Strength_Gene() : piece_strength{}
{
    recalculate_normalizing_value();
}

std::map<std::string, double> Piece_Strength_Gene::list_properties() const
{
    std::map<std::string, double> properties;
    for(auto piece_type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
    {
        auto piece = Board::piece_instance(piece_type, WHITE);
        properties[std::string(1, piece->fen_symbol())] = piece_value(piece);
    }
    return properties;
}

void Piece_Strength_Gene::load_properties(const std::map<std::string, double>& properties)
{
    for(const auto& piece_score : properties)
    {
        Piece_Type piece;
        switch(piece_score.first[0])
        {
            case 'P':
                piece = PAWN;
                break;
            case 'R':
                piece = ROOK;
                break;
            case 'N':
                piece = KNIGHT;
                break;
            case 'B':
                piece = BISHOP;
                break;
            case 'Q':
                piece = QUEEN;
                break;
            case 'K':
                piece = KING;
                break;
            default:
                throw std::runtime_error("Unrecognized piece type: " + piece_score.first);
        }
        piece_value(piece) = piece_score.second;
    }

    recalculate_normalizing_value();
}

void Piece_Strength_Gene::gene_specific_mutation()
{
    auto index = Random::random_integer(0, int(piece_strength.size()) - 1);
    piece_strength[index] += Random::random_laplace(1.0);
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

//! This method is queried by other Genes to determine the value of various Pieces.

//! \param piece A pointer to a Piece. A nullptr represents no piece (as from an empty square).
//! \returns A numerical value of the Piece.
double Piece_Strength_Gene::piece_value(const Piece* piece) const
{
    if( ! piece)
    {
        return 0.0;
    }
    else
    {
        return piece_value(piece->type());
    }
}

//! Returns the total value of the pieces on the board at the start of a standard game.

//! \returns A value used by other Genes to normalize their scores.
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
