#include "Genes/Last_Minute_Panic_Gene.h"

#include <string>

#include "Utility.h"
#include "Game/Board.h"
#include "Game/Color.h"

Last_Minute_Panic_Gene::Last_Minute_Panic_Gene() :
    Gene(0.0),
    minimum_time(0)
{
}

void Last_Minute_Panic_Gene::reset_properties() const
{
    properties["Minimum time required"] = minimum_time;
}

void Last_Minute_Panic_Gene::load_properties()
{
    minimum_time = properties["Minimum time required"];
}

Last_Minute_Panic_Gene::~Last_Minute_Panic_Gene()
{
}

void Last_Minute_Panic_Gene::mutate()
{
    minimum_time = std::max(minimum_time + Random::random_normal(1.0), 0.0);
}

std::string Last_Minute_Panic_Gene::name() const
{
    return "Last Minute Panic Gene";
}

Last_Minute_Panic_Gene* Last_Minute_Panic_Gene::duplicate() const
{
    return new Last_Minute_Panic_Gene(*this);
}

double Last_Minute_Panic_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}

double Last_Minute_Panic_Gene::time_required() const
{
    return minimum_time;
}
