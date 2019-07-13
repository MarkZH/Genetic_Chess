#include "Moves/Castle.h"

#include <string>

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"

Castle::Castle(int base_rank, Direction direction) :
    Move({'e', base_rank},
         {(direction == RIGHT ? 'g' : 'c'), base_rank}),
    rook_move({(direction == RIGHT ? 'h' : 'a'), base_rank},
              {(direction == RIGHT ? 'f' : 'd'), base_rank})
{
    able_to_capture = false;
    is_castling_move = true;
}

bool Castle::move_specific_legal(const Board& board) const
{
    return     ! board.piece_has_moved(start())
            && ! board.piece_has_moved(rook_move.start())
            && ! board.king_is_in_check()
            && board.safe_for_king(rook_move.end(), board.whose_turn())
            && board.all_empty_between(start(), rook_move.start());
}

Square Castle::rook_end_square() const
{
    return rook_move.end();
}

void Castle::side_effects(Board& board) const
{
    board.move_piece(rook_move);
    board.castling_index[board.whose_turn()] = board.game_record().size() - 1;
}

std::string Castle::game_record_move_item(const Board&) const
{
    return file_change() > 0 ? "O-O" : "O-O-O";
}
