#include "Genes/Checkmate_Material_Gene.h"

#include <memory>
#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

double Checkmate_Material_Gene::score_board(const Board& board, Color perspective, size_t) const noexcept
{
    return board.enough_material_to_checkmate(perspective);
}

std::unique_ptr<Gene> Checkmate_Material_Gene::duplicate() const noexcept
{
    return std::make_unique<Checkmate_Material_Gene>(*this);
}

std::string Checkmate_Material_Gene::name() const noexcept
{
    return "Checkmate Material Gene";
}
