#include "Genes/Freedom_To_Move_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Complete_Move.h"

Freedom_To_Move_Gene::Freedom_To_Move_Gene() :
    initial_number_of_moves(Board().legal_moves().size())
{
}

double Freedom_To_Move_Gene::score_board(const Board& board, Color perspective) const
{
    auto temp = board;
    temp.set_turn(perspective);
    return double(temp.legal_moves().size())/initial_number_of_moves;
}

Freedom_To_Move_Gene* Freedom_To_Move_Gene::duplicate() const
{
    return new Freedom_To_Move_Gene(*this);
}

std::string Freedom_To_Move_Gene::name() const
{
    return "Freedom to Move Gene";
}
