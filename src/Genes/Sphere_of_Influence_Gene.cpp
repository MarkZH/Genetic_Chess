#include "Genes/Sphere_of_Influence_Gene.h"

#include <algorithm>

#include "Game/Board.h"
#include "Moves/Move.h"

#include "Utility.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene() : legal_bonus(1.0)
{
}

Sphere_of_Influence_Gene::~Sphere_of_Influence_Gene()
{
}

void Sphere_of_Influence_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Legal Bonus"] = legal_bonus;
}

void Sphere_of_Influence_Gene::load_properties()
{
    Gene::load_properties();
    legal_bonus = properties["Legal Bonus"];
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
    auto temp = board.make_hypothetical();
    temp.set_turn(perspective);

    for(const auto& cm : temp.all_moves())
    {
        // Only potentially capturing moves are counted
        if( ! cm.move->can_capture())
        {
            continue;
        }

        char final_file = cm.starting_file + cm.move->file_change();
        int  final_rank = cm.starting_rank + cm.move->rank_change();

        if( ! Board::inside_board(final_file, final_rank))
        {
            continue;
        }

        Square square_address{final_file, final_rank};
        square_score[square_address] = 1;
    }

    for(const auto& cm : temp.all_legal_moves())
    {
        // Only potentially capturing moves are counted
        if( ! cm.move->can_capture())
        {
            continue;
        }

        char final_file = cm.starting_file + cm.move->file_change();
        int  final_rank = cm.starting_rank + cm.move->rank_change();

        Square square_address{final_file, final_rank};
        square_score[square_address] = legal_bonus;
    }

    double score = 0;
    for(const auto& square_value : square_score)
    {
        score += square_value.second;
    }

    // maximum board_score is 1 (normalizing to make independent of scalar)
    return score/(64.0*legal_bonus);
}


void Sphere_of_Influence_Gene::mutate()
{
    Gene::mutate();
    legal_bonus = std::max(legal_bonus + Random::random_normal(1.0), 1.0);
}
