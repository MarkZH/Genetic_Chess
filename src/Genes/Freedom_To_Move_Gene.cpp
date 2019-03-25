#include "Genes/Freedom_To_Move_Gene.h"

#include <memory>
#include <string>
#include <vector>

#include "Genes/Gene.h"
#include "Game/Board.h"

double Freedom_To_Move_Gene::score_board(const Board& board, const Board&, size_t) const
{
    static auto initial_number_of_moves = double(Board().legal_moves().size());
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
