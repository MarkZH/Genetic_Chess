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
    legal_bonus(1.0),
    king_target_factor(0.0)
{
}

void Sphere_of_Influence_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Legal Bonus"] = legal_bonus;
    properties["King Target Factor"] = king_target_factor;
}

void Sphere_of_Influence_Gene::load_properties()
{
    Gene::load_properties();
    legal_bonus = properties["Legal Bonus"];
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
double Sphere_of_Influence_Gene::score_board(const Board& board) const
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

    double score_to_add = 1.0;
    for(const auto& move_list : {board.other_moves(), board.legal_moves()})
    {
        for(const auto& move : move_list)
        {
            // Only potentially capturing moves are counted
            if( ! move->can_capture())
            {
                continue;
            }

            char final_file = move->end_file();
            int  final_rank = move->end_rank();

            if(move->is_en_passant())
            {
                if(board.is_en_passant_targetable(final_file, final_rank))
                {
                    final_rank -= move->rank_change();
                }
                else
                {
                    continue;
                }
            }

            square_score[Board::board_index(final_file, final_rank)] = score_to_add;
        }

        score_to_add = legal_bonus; // now on legal move list
    }

    double score = 0;
    for(size_t i = 0; i < square_score.size(); ++i)
    {
        auto king_bonus = king_target_factor/(1 + distance_to_king[i]);
        score += square_score[i]*(1 + king_bonus);
    }

    // normalizing to make maximum score near 1
    return score/64.0;
}


void Sphere_of_Influence_Gene::gene_specific_mutation()
{
    if(Random::coin_flip())
    {
        legal_bonus += Random::random_normal(0.05);
    }
    else
    {
        king_target_factor += Random::random_normal(0.05);
    }
}
