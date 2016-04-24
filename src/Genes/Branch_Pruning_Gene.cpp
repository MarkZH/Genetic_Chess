#include "Branch_Pruning_Gene.h"

#include "Utility.h"


Branch_Pruning_Gene::Branch_Pruning_Gene() :
    Gene(0.0),
    minimum_score_change_limit(0)
{
    reset_properties();
}

void Branch_Pruning_Gene::reset_properties()
{
    properties["Minimum Score Change"] = &minimum_score_change_limit;
}

Branch_Pruning_Gene::~Branch_Pruning_Gene()
{
}

void Branch_Pruning_Gene::mutate()
{
    minimum_score_change_limit += -0.5 + Random::random_normal(5.0);
}

Branch_Pruning_Gene* Branch_Pruning_Gene::duplicate() const
{
    auto dupe = new Branch_Pruning_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string Branch_Pruning_Gene::name() const
{
    return "Branch Pruning Gene";
}

double Branch_Pruning_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}

double Branch_Pruning_Gene::minimum_score_change() const
{
    return minimum_score_change_limit;
}
