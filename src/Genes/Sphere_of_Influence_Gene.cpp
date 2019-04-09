#include "Genes/Sphere_of_Influence_Gene.h"

#include <cmath>
#include <array>
#include <memory>
#include <map>
#include <array>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Moves/Move.h"

#include "Utility/Random.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() :
    legal_square_score(1.0),
    illegal_square_score(1.0),
    king_target_factor(0.0)
{
    recompute_scalar_cache();
}

std::map<std::string, double> Sphere_of_Influence_Gene::list_properties() const
{
    auto properties = Gene::list_properties();
    properties["Legal Square Score"] = legal_square_score;
    properties["Illegal Square Score"] = illegal_square_score;
    properties["King Target Factor"] = king_target_factor;
    return properties;
}

void Sphere_of_Influence_Gene::load_properties(const std::map<std::string, double>& properties)
{
    Gene::load_properties(properties);
    legal_square_score = properties.at("Legal Square Score");
    illegal_square_score = properties.at("Illegal Square Score");
    king_target_factor = properties.at("King Target Factor");
    recompute_scalar_cache();
}

std::unique_ptr<Gene> Sphere_of_Influence_Gene::duplicate() const
{
    return std::make_unique<Sphere_of_Influence_Gene>(*this);
}

std::string Sphere_of_Influence_Gene::name() const
{
    return "Sphere of Influence Gene";
}

// Count all squares potentially attacked by all pieces with bonus points if
// the attacking move is legal.
double Sphere_of_Influence_Gene::score_board(const Board& board, const Board&, size_t) const
{
    const auto& legal_attacks = board.all_square_indices_attacked();
    const auto& illegal_attacks = board.other_square_indices_attacked();
    const auto& opponent_king_square = board.find_king(opposite(board.whose_turn()));

    double score = 0;
    for(size_t i = 0; i < legal_attacks.size(); ++i)
    {
        double square_score;
        if(legal_attacks[i])
        {
            square_score = legal_square_score;
        }
        else if(illegal_attacks[i])
        {
            square_score = illegal_square_score;
        }
        else
        {
            continue;
        }

        auto distance_to_king = king_distance(Square{i}, opponent_king_square);
        score += square_score*scalar_cache[distance_to_king];
    }

    return score;
}


void Sphere_of_Influence_Gene::gene_specific_mutation()
{
    make_priority_minimum_zero();

    switch(Random::random_integer(1, 3))
    {
        case 1:
            legal_square_score += Random::random_laplace(0.5);
            break;
        case 2:
            illegal_square_score += Random::random_laplace(0.5);
            break;
        case 3:
            king_target_factor += Random::random_laplace(0.5);
            break;
        default:
            throw std::runtime_error("Bad range in Sphere_of_Influence_Gene::gene_specific_mutation()");
    }

    recompute_scalar_cache();
}

void Sphere_of_Influence_Gene::recompute_scalar_cache()
{
    for(int king_distance = 0; king_distance < 8; ++king_distance)
    {
        scalar_cache[king_distance] = (1 + king_target_factor/(1 + king_distance))/(64*(std::abs(legal_square_score) + std::abs(illegal_square_score)));
    }
}
