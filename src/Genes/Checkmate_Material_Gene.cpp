#include "Genes/Checkmate_Material_Gene.h"

#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

double Checkmate_Material_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    return board.enough_material_to_checkmate(perspective);
}

std::string Checkmate_Material_Gene::name() const noexcept
{
    return "Checkmate Material Gene";
}
