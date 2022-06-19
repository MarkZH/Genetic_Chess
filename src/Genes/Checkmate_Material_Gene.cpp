#include "Genes/Checkmate_Material_Gene.h"

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

Checkmate_Material_Gene::Checkmate_Material_Gene() noexcept : Clonable_Gene("Checkmate Material Gene")
{
}

double Checkmate_Material_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    return board.enough_material_to_checkmate(perspective);
}
