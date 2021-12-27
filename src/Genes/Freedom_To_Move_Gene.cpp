#include "Genes/Freedom_To_Move_Gene.h"

#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

double Freedom_To_Move_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    return perspective == board.whose_turn() ? double(board.legal_moves().size())/128.0 : 0.0;
}

std::string Freedom_To_Move_Gene::name() const noexcept
{
    return "Freedom to Move Gene";
}
