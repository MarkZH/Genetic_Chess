#include "Genes/Checkmate_Material_Gene.h"

#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"

double Checkmate_Material_Gene::score_board(const Board& board, const Board&, size_t) const
{
    return board.enough_material_to_checkmate(board.whose_turn());
}

std::unique_ptr<Gene> Checkmate_Material_Gene::duplicate() const
{
    return std::make_unique<Checkmate_Material_Gene>(*this);
}

std::string Checkmate_Material_Gene::name() const
{
    return "Checkmate Material Gene";
}
