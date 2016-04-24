#include "Genes/Freedom_To_Move_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"

Freedom_To_Move_Gene::Freedom_To_Move_Gene() : Gene(1.0)
{
    reset_properties();
}

void Freedom_To_Move_Gene::reset_properties()
{
    reset_base_properties();
}

double Freedom_To_Move_Gene::score_board(const Board& board, Color color) const
{
    auto temp = board.make_hypothetical();
    temp.set_turn(color);
    return temp.all_legal_moves().size();
}

Freedom_To_Move_Gene* Freedom_To_Move_Gene::duplicate() const
{
    auto dupe = new Freedom_To_Move_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string Freedom_To_Move_Gene::name() const
{
    return "Freedom to Move Gene";
}
