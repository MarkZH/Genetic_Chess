#include "Genes/Look_Ahead_Gene.h"

#include <cmath>

#include "Utility.h"


Look_Ahead_Gene::Look_Ahead_Gene() :
    Gene(0.0),
    look_ahead_constant(0)
{
}

void Look_Ahead_Gene::reset_properties() const
{
    properties["Look Ahead Constant"] = look_ahead_constant;
}

void Look_Ahead_Gene::load_properties()
{
    look_ahead_constant = properties["Look Ahead Constant"];
}

Look_Ahead_Gene::~Look_Ahead_Gene()
{
}

size_t Look_Ahead_Gene::positions_to_examine(double time_left) const
{
    return std::max(look_ahead_constant*time_left, 0.0);
}

void Look_Ahead_Gene::mutate()
{
    look_ahead_constant = std::max(0.0, look_ahead_constant + Random::random_normal(5.0));
}

Look_Ahead_Gene* Look_Ahead_Gene::duplicate() const
{
    return new Look_Ahead_Gene(*this);
}

std::string Look_Ahead_Gene::name() const
{
    return "Look Ahead Gene";
}

double Look_Ahead_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}
