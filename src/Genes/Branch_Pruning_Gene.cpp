#include "Genes/Branch_Pruning_Gene.h"

#include "Utility.h"

Branch_Pruning_Gene::Branch_Pruning_Gene() :
    minimum_score_change(0.0)
{
}

Branch_Pruning_Gene::~Branch_Pruning_Gene()
{
}

void Branch_Pruning_Gene::reset_properties() const
{
    properties["Minimum Score Change"] = minimum_score_change;
}

void Branch_Pruning_Gene::load_properties()
{
    minimum_score_change = properties["Minimum Score Change"];
}

void Branch_Pruning_Gene::gene_specific_mutation()
{
    minimum_score_change += Random::random_normal(5.0);
}

Branch_Pruning_Gene* Branch_Pruning_Gene::duplicate() const
{
    return new Branch_Pruning_Gene(*this);
}

std::string Branch_Pruning_Gene::name() const
{
    return "Branch Pruning Gene";
}

double Branch_Pruning_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}

bool Branch_Pruning_Gene::good_enough_to_examine(double score_difference) const
{
    if(is_active())
    {
        return score_difference > minimum_score_change;
    }
    else
    {
        return true;
    }
}
