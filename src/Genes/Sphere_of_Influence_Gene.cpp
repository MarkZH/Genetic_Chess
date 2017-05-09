#include "Genes/Sphere_of_Influence_Gene.h"

#include <algorithm>

#include "Game/Board.h"
#include "Moves/Complete_Move.h"

#include "Utility.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() :
    legal_bonus(1.0),
    king_target_factor(0.0)
{
}

Sphere_of_Influence_Gene::~Sphere_of_Influence_Gene()
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

Sphere_of_Influence_Gene* Sphere_of_Influence_Gene::duplicate() const
{
    return new Sphere_of_Influence_Gene(*this);
}

std::string Sphere_of_Influence_Gene::name() const
{
    return "Sphere of Influence Gene";
}

// Count all squares potentially attacked by all pieces with bonus points if
// the attacking move is legal.
double Sphere_of_Influence_Gene::score_board(const Board& board, Color perspective) const
{
    std::map<Square, double> square_score;
    auto temp = board;
    temp.set_turn(perspective);
    auto opponent_king_square = board.find_king(opposite(perspective));

    double score_to_add = 1.0;
    for(const auto& move_list : {temp.other_moves(), temp.legal_moves()})
    {
        for(const auto& cm : move_list)
        {
            // Only potentially capturing moves are counted
            if( ! cm.can_capture())
            {
                continue;
            }

            char final_file = cm.end_file();
            int  final_rank = cm.end_rank();

            if(cm.name().front() == 'E')
            {
                if(temp.is_en_passant_targetable(final_file, final_rank))
                {
                    final_rank += (temp.whose_turn() == WHITE ? -1 : 1);
                }
                else
                {
                    continue;
                }
            }

            square_score[{final_file, final_rank}] = score_to_add;
        }

        score_to_add = legal_bonus; // now on legal move list
    }

    double score = 0;
    for(const auto& square_value : square_score)
    {
        score += square_value.second +
                 king_target_factor/(1 + king_distance(opponent_king_square, square_value.first));
    }

    // normalizing to make maximum score near 1
    return score/64.0;
}


void Sphere_of_Influence_Gene::gene_specific_mutation()
{
    legal_bonus += Random::random_normal(0.05);
    king_target_factor += Random::random_normal(0.05);
}
