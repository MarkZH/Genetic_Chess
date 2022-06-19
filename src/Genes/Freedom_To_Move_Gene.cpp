#include "Genes/Freedom_To_Move_Gene.h"

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

Freedom_To_Move_Gene::Freedom_To_Move_Gene() noexcept : Clonable_Gene("Freedom to Move Gene")
{
}

double Freedom_To_Move_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    return perspective == board.whose_turn() ? double(board.legal_moves().size())/128.0 : 0.0;
}
