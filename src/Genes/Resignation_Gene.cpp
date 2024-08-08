#include "Resignation_Gene.h"

#include "Gene_Value.h"
#include "Utility/Random.h"
#include "Game/Color.h"
#include "Players/Game_Tree_Node_Result.h"

Resignation_Gene::Resignation_Gene() noexcept : Clonable_Gene("Resignation Gene")
{
}

bool Resignation_Gene::should_resign(const std::vector<Game_Tree_Node_Result>& commentary, const Piece_Color perspective) const noexcept
{
    auto under_floor_streak = 0;
    for(auto it = commentary.rbegin(); it != commentary.rend(); ++it)
    {
        if(it->corrected_score(perspective) < board_score_floor.value())
        {
            ++under_floor_streak;
            if(under_floor_streak > max_under_floor_streak.value())
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    return false;
}

void Resignation_Gene::gene_specific_mutation() noexcept
{
    if(Random::coin_flip())
    {
        board_score_floor.mutate();
    }
    else
    {
        max_under_floor_streak.mutate();
        if(max_under_floor_streak.value() < 0.0)
        {
            max_under_floor_streak.value() *= -1;
        }
    }
}

void Resignation_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    delete_activations(properties);
    delete_priorities(properties);
    board_score_floor.write_to_map(properties);
    max_under_floor_streak.write_to_map(properties);
}

void Resignation_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    board_score_floor.load_from_map(properties);
    max_under_floor_streak.load_from_map(properties);
}

double Resignation_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}
