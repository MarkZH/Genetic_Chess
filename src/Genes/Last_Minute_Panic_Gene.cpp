#include "Last_Minute_Panic_Gene.h"

#include <string>

#include "Utility.h"
#include "Game/Board.h"
#include "Game/Color.h"

Last_Minute_Panic_Gene::Last_Minute_Panic_Gene() :
    Gene(0.0),
    minimum_time(0)
{
    reset_properties();
}

void Last_Minute_Panic_Gene::reset_properties()
{
    properties["Minimum time required"] = &minimum_time;
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
    auto dupe = new Last_Minute_Panic_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

double Last_Minute_Panic_Gene::score_board(const Board&, Color) const
{
    return 0.0;
}

double Last_Minute_Panic_Gene::time_required() const
{
    return minimum_time;
}
