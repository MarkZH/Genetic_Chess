#include "Genes/Sphere_of_Influence_Gene.h"

#include <memory>
#include <algorithm>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Color.h"

std::unique_ptr<Gene> Sphere_of_Influence_Gene::duplicate() const
{
    return std::make_unique<Sphere_of_Influence_Gene>(*this);
}

std::string Sphere_of_Influence_Gene::name() const
{
    return "Sphere of Influence Gene";
}

double Sphere_of_Influence_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    return std::count_if(Square::all_squares().begin(), Square::all_squares().end(),
                         [&board, perspective](auto square)
                         {
                             return ! board.safe_for_king(square, opposite(perspective)); // any piece attacks square
                         })/64.0;
}
