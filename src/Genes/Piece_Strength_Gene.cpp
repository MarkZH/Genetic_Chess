#include "Genes/Piece_Strength_Gene.h"

#include <array>
#include <memory>

#include "Genes/Gene.h"
#include "Utility.h"
#include "Game/Piece.h"
#include "Game/Board.h"

Piece_Strength_Gene::Piece_Strength_Gene() : piece_strength{}
{
}

void Piece_Strength_Gene::reset_properties() const
{
    for(auto piece_type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
    {
        auto piece = Board::piece_instance(piece_type, WHITE);
        properties[std::string(1, piece->fen_symbol())] = piece_value(piece);
    }
}

void Piece_Strength_Gene::load_properties()
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
}

void Piece_Strength_Gene::gene_specific_mutation()
{
    for(auto& piece_score : piece_strength)
    {
        const double mean_number_of_mutations = 2.0;
        if(Random::success_probability(mean_number_of_mutations/piece_strength.size()))
        {
            piece_score += Random::random_laplace(1.0);
        }
    }
}

double Piece_Strength_Gene::piece_value(Piece_Type type) const
{
    return piece_strength[type];
}

double& Piece_Strength_Gene::piece_value(Piece_Type type)
{
    return piece_strength[type];
}

double Piece_Strength_Gene::piece_value(const Piece* piece) const
{
    if( ! piece)
    {
        return 0.0;
    }
    else
    {
        // Sum is equal to the total strength of a player's starting pieces
        // (8 pawns, 2 rooks, 2 knights, 2 bishops, 1 queen, and 1 king).
        auto normalizing_factor = 8*std::abs(piece_value(PAWN)) +
                                  2*std::abs(piece_value(ROOK)) +
                                  2*std::abs(piece_value(KNIGHT)) +
                                  2*std::abs(piece_value(BISHOP)) +
                                    std::abs(piece_value(QUEEN)) +
                                    std::abs(piece_value(KING));

        return piece_value(piece->type())/normalizing_factor;
    }
}

std::unique_ptr<Gene> Piece_Strength_Gene::duplicate() const
{
    return std::make_unique<Piece_Strength_Gene>(*this);
}

std::string Piece_Strength_Gene::name() const
{
    return "Piece Strength Gene";
}

double Piece_Strength_Gene::score_board(const Board&, const Board&, size_t) const
{
    return 0.0;
}
