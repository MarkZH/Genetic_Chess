#include "Genes/Priority_Threshold_Gene.h"

#include <memory>
#include <string>
#include <algorithm>

#include "Utility.h"

class Board;

Priority_Threshold_Gene::Priority_Threshold_Gene() : threshold(0.0)
{
}

void Priority_Threshold_Gene::reset_properties() const
{
    properties["Threshold"] = threshold;
}

void Priority_Threshold_Gene::load_properties()
{
    threshold = properties["Threshold"];
}

std::unique_ptr<Gene> Priority_Threshold_Gene::duplicate() const
{
    return std::make_unique<Priority_Threshold_Gene>(*this);
}

std::string Priority_Threshold_Gene::name() const
{
    return "Priority Threshold Gene";
}

double Priority_Threshold_Gene::score_board(const Board&) const
{
    return 0.0;
}

double Priority_Threshold_Gene::get_threshold() const
{
    return threshold;
}

void Priority_Threshold_Gene::gene_specific_mutation()
{
    threshold = std::max(0.0, threshold + Random::random_normal(2.0));
}
