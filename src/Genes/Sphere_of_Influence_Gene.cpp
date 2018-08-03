#include "Genes/Sphere_of_Influence_Gene.h"

#include <algorithm>
#include <array>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Moves/Move.h"

#include "Utility.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() :
    legal_square_score(1.0),
    illegal_square_score(1.0),
    king_target_factor(0.0)
{
}

void Sphere_of_Influence_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Legal Square Score"] = legal_square_score;
    properties["Illegal Square Score"] = illegal_square_score;
    properties["King Target Factor"] = king_target_factor;
}

void Sphere_of_Influence_Gene::load_properties()
{
    Gene::load_properties();
    legal_square_score = properties["Legal Square Score"];
    illegal_square_score = properties["Illegal Square Score"];
    king_target_factor = properties["King Target Factor"];
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
double Sphere_of_Influence_Gene::score_board(const Board& board, const Board&) const
{
    std::array<double, 64> square_score{};
    std::array<size_t, 64> distance_to_king{};
    auto opponent_king_square = board.find_king(opposite(board.whose_turn()));
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            distance_to_king[Board::board_index(file, rank)] =
                king_distance({file, rank}, opponent_king_square);
        }
    }

    double score_to_add = illegal_square_score;
    for(const auto& attack_list : {board.other_square_indices_attacked(), board.all_square_indices_attacked()})
    {
        for(size_t i = 0; i < attack_list.size(); ++i)
        {
            if(attack_list[i])
            {
                square_score[i] = score_to_add;
            }
        }

        score_to_add = legal_square_score; // now on legal move list
    }

    double score = 0;
    for(size_t i = 0; i < square_score.size(); ++i)
    {
        auto king_bonus = king_target_factor/(1 + distance_to_king[i]);
        score += square_score[i]*(1 + king_bonus);
    }

    // normalizing to make maximum score near 1
    return score/(64*(std::abs(legal_square_score) + std::abs(illegal_square_score)));
}


void Sphere_of_Influence_Gene::gene_specific_mutation()
{
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
}
