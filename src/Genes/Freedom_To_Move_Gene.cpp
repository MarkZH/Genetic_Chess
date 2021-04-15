#include "Genes/Freedom_To_Move_Gene.h"

#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

double Freedom_To_Move_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    constexpr auto maximum_moves_per_turn = 128.0;
    if(perspective == board.whose_turn())
    {
        return double(board.legal_moves().size())/maximum_moves_per_turn;
    }
    else
    {
        return double(board.previous_moves_count())/maximum_moves_per_turn;
    }
}

std::string Freedom_To_Move_Gene::name() const noexcept
{
    return "Freedom to Move Gene";
}
