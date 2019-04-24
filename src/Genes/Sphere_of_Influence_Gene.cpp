#include "Genes/Sphere_of_Influence_Gene.h"

#include <cmath>
#include <array>
#include <memory>
#include <map>
#include <array>
#include <mutex>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Moves/Move.h"
#include "Game/Color.h"

#include "Utility/Random.h"

std::array<std::array<size_t, 64>, 64> Sphere_of_Influence_Gene::king_distances{};
bool Sphere_of_Influence_Gene::king_distances_initialized = false;

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() :
    legal_square_score(1.0),
    illegal_square_score(1.0),
    king_target_factor(0.0)
{
    static std::mutex m;
    auto lock = std::lock_guard<std::mutex>(m);

    if( ! king_distances_initialized)
    {
        for(char file_a = 'a'; file_a <= 'h'; ++file_a)
        {
            for(int rank_a = 1; rank_a <= 8; ++rank_a)
            {
                for(char file_b = 'a'; file_b <= 'h'; ++file_b)
                {
                    for(int rank_b = 1; rank_b <= 8; ++rank_b)
                    {
                        auto index_a = Board::square_index(file_a, rank_a);
                        auto index_b = Board::square_index(file_b, rank_b);
                        king_distances[index_a][index_b] =
                            size_t(std::max(std::abs(file_a - file_b), std::abs(rank_a - rank_b)));
                    }

                }
            }
        }

        king_distances_initialized = true;
    }

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
double Sphere_of_Influence_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    const auto& opponent_king_square = board.find_king(opposite(perspective));
    auto opponent_king_index = board.square_index(opponent_king_square.file(), opponent_king_square.rank());

    double score = 0;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            double square_score;
            if( ! board.safe_for_king(file, rank, opposite(perspective))) // any piece attacks square
            {
                square_score = legal_square_score;
            }
            else if(board.blocked_attack(file, rank, perspective)) // There are attacks on this square blocked by other pieces
            {
                square_score = illegal_square_score;
            }
            else
            {
                continue;
            }

            auto distance_to_king = king_distances[board.square_index(file, rank)][opponent_king_index];
            score += square_score*scalar_cache[distance_to_king];
        }
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
            king_target_factor += Random::random_laplace(0.1);
            break;
        default:
            throw std::logic_error("Bad range in Sphere_of_Influence_Gene::gene_specific_mutation()");
    }

    recompute_scalar_cache();
}

void Sphere_of_Influence_Gene::recompute_scalar_cache()
{
    for(size_t king_distance = 0; king_distance < 8; ++king_distance)
    {
        scalar_cache[king_distance] = (1 + king_target_factor/(1 + king_distance))/(64*(std::abs(legal_square_score) + std::abs(illegal_square_score)));
    }
}
