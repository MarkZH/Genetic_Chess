#include "Genes/Null_Gene.h"

#include <memory>
#include <string>

#include "Genes/Gene.h"
#include "Game/Color.h"

class Board;

double Null_Gene::score_board(const Board&, Color, size_t) const
{
    return 0.0;
}

std::unique_ptr<Gene> Null_Gene::duplicate() const
{
    return std::make_unique<Null_Gene>(*this);
}

std::string Null_Gene::name() const
{
    return "Null Gene";
}
