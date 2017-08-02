#include "Genes/Freedom_To_Move_Gene.h"

#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Move.h"

Freedom_To_Move_Gene::Freedom_To_Move_Gene() :
    initial_number_of_moves(Board().legal_moves().size())
{
}

double Freedom_To_Move_Gene::score_board(const Board& board) const
{
    return board.legal_moves().size()/initial_number_of_moves;
}

std::unique_ptr<Gene> Freedom_To_Move_Gene::duplicate() const
{
    return std::make_unique<Freedom_To_Move_Gene>(*this);
}

std::string Freedom_To_Move_Gene::name() const
{
    return "Freedom to Move Gene";
}
