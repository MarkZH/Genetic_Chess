#include "Genes/Sphere_of_Influence_Gene.h"

#include <cmath>
#include <map>

#include "Genes/Gene.h"
#include "Genes/Interpolated_Gene_Value.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

namespace
{
    const auto inv_king_dist_plus_one =
        []()
        {
            std::array<std::array<double, 64>, 64> result;
            for(const auto square_a : Square::all_squares())
            {
                for(const auto square_b : Square::all_squares())
                {
                    // The "king distance" between two squares is the minimum number
                    // of moves a king needs to get from one square to the other.
                    const auto diff = square_a - square_b;
                    result[square_a.index()][square_b.index()] =
                        1.0/(1.0 + std::max(std::abs(diff.file_change), std::abs(diff.rank_change)));
                }
            }
            return result;
        }();
}

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() noexcept
{
    normalize_square_scores();
}

void Sphere_of_Influence_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    legal_square_scores.write_to_map(properties);
    illegal_square_scores.write_to_map(properties);
    king_target_factors.write_to_map(properties);
}

void Sphere_of_Influence_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    legal_square_scores.load_from_map(properties);
    illegal_square_scores.load_from_map(properties);
    king_target_factors.load_from_map(properties);
    normalize_square_scores();
}

std::string Sphere_of_Influence_Gene::name() const noexcept
{
    return "Sphere of Influence Gene";
}

// Count all squares potentially attacked by all pieces with bonus points if
// the attacking move is legal.
double Sphere_of_Influence_Gene::score_board(const Board& board, Piece_Color perspective, size_t, double game_progress) const noexcept
{
    const auto legal_square_score = legal_square_scores.interpolate(game_progress);
    const auto illegal_square_score = illegal_square_scores.interpolate(game_progress);
    const auto king_target_factor = king_target_factors.interpolate(game_progress);
    const auto opponent_king_square = board.find_king(opposite(perspective));
    const auto& inv_dist_plus_one = inv_king_dist_plus_one[opponent_king_square.index()];

    double score = 0.0;
    for(auto square : Square::all_squares())
    {
        double square_score;
        if(board.attacked_by(square, perspective)) // any piece attacks square
        {
            square_score = legal_square_score;
        }
        else if(board.blocked_attack(square, perspective)) // There are attacks on this square blocked by other pieces
        {
            square_score = illegal_square_score;
        }
        else
        {
            continue;
        }

        auto inv_dist_to_king_plus_one = inv_dist_plus_one[square.index()];
        score += square_score*(1 + king_target_factor*inv_dist_to_king_plus_one);
    }

    return score/64;
}


void Sphere_of_Influence_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 3))
    {
        case 1:
            legal_square_scores.mutate(0.03);
            break;
        case 2:
            king_target_factors.mutate(0.1);
            break;
        case 3:
            illegal_square_scores.mutate(0.03);
            break;
        default:
            assert(false);
    }

    normalize_square_scores();
}

void Sphere_of_Influence_Gene::normalize_square_scores() noexcept
{
    Math::normalize(legal_square_scores.opening_value(), illegal_square_scores.opening_value());
    Math::normalize(legal_square_scores.endgame_value(), illegal_square_scores.endgame_value());
}
