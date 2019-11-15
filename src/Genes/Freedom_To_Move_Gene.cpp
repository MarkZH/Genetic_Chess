#include "Genes/Freedom_To_Move_Gene.h"

#include <memory>
#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

double Freedom_To_Move_Gene::score_board(const Board& board, Color perspective, size_t) const noexcept
{
    static auto initial_score = double(Board().legal_moves().size());
    if(perspective == board.whose_turn())
    {
        return board.legal_moves().size()/initial_score;
    }
    else
    {
        return board.previous_moves_count()/initial_score;
    }
}

std::unique_ptr<Gene> Freedom_To_Move_Gene::duplicate() const noexcept
{
    return std::make_unique<Freedom_To_Move_Gene>(*this);
}

std::string Freedom_To_Move_Gene::name() const noexcept
{
    return "Freedom to Move Gene";
}
