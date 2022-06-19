#include "Genes/Sphere_of_Influence_Gene.h"

#include <map>

#include "Genes/Gene.h"
#include "Genes/Interpolated_Gene_Value.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() noexcept : Clonable_Gene("Sphere of Influence Gene")
{
    normalize_square_scores();
}

void Sphere_of_Influence_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    legal_square_score.write_to_map(properties);
    illegal_square_score.write_to_map(properties);
}

void Sphere_of_Influence_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    legal_square_score.load_from_map(properties);
    illegal_square_score.load_from_map(properties);
    normalize_square_scores();
}

double Sphere_of_Influence_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    double score = 0.0;
    for(const auto square : Square::all_squares())
    {
        double square_score;
        if(board.attacked_by(square, perspective)) // any piece attacks square
        {
            square_score = legal_square_score.value();
        }
        else if(board.blocked_attack(square, perspective)) // There are attacks on this square blocked by other pieces
        {
            square_score = illegal_square_score.value();
        }
        else
        {
            continue;
        }

        const auto invasion_distance = (perspective == Piece_Color::WHITE ? square.rank() : 9 - square.rank());
        score += square_score*invasion_distance;
    }

    return score/288;
}


void Sphere_of_Influence_Gene::gene_specific_mutation() noexcept
{
    if(Random::coin_flip())
    {
        legal_square_score.mutate();
    }
    else
    {
        illegal_square_score.mutate();
    }

    normalize_square_scores();
}

void Sphere_of_Influence_Gene::normalize_square_scores() noexcept
{
    Math::normalize(legal_square_score.value(), illegal_square_score.value());
}
