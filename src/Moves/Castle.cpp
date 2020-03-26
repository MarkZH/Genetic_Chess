#include "Moves/Castle.h"

#include <string>

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"

Castle::Castle(int base_rank, Direction direction) noexcept :
    Move({'e', base_rank},
         {(direction == Direction::RIGHT ? 'g' : 'c'), base_rank}),
    rook_move({(direction == Direction::RIGHT ? 'h' : 'a'), base_rank},
              {(direction == Direction::RIGHT ? 'f' : 'd'), base_rank})
{
    set_capturing_ability(false);
}

bool Castle::move_specific_legal(const Board& board) const noexcept
{
    return     ! board.piece_has_moved(start())
            && ! board.piece_has_moved(rook_move.start())
            && ! board.king_is_in_check()
            && board.safe_for_king(rook_move.end(), board.whose_turn())
            && board.all_empty_between(start(), rook_move.start());
}

void Castle::side_effects(Board& board) const noexcept
{
    board.move_piece(rook_move);
    board.castling_index[static_cast<int>(board.whose_turn())] = board.game_length() - 1;
    board.castling_movement[static_cast<int>(board.whose_turn())] = file_change();
}

std::string Castle::algebraic_base(const Board&) const noexcept
{
    return file_change() > 0 ? "O-O" : "O-O-O";
}
