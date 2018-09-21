#include "Moves/Castle.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Castle::Castle(int base_rank, Direction direction) :
    Move('e',
         base_rank,
         (direction == RIGHT ? 'g' : 'c'),
         base_rank),
    rook_starting_file(direction == RIGHT ? 'h' : 'a'),
    rook_ending_file(direction == RIGHT ? 'f' : 'd')
{
    able_to_capture = false;
}

bool Castle::move_specific_legal(const Board& board) const
{
    auto skipped_file = (starting_file + ending_file)/2;
    return     ! board.piece_has_moved(starting_file, starting_rank)
            && ! board.piece_has_moved(rook_starting_file, starting_rank)
            && ! board.king_is_in_check()
            && board.all_empty_between(starting_file, starting_rank, rook_starting_file, starting_rank)
            && board.safe_for_king(skipped_file, starting_rank, board.whose_turn());
}

void Castle::side_effects(Board& board) const
{
    board.make_move(rook_starting_file, starting_rank, rook_ending_file, starting_rank); // move Rook
    board.castling_index[board.whose_turn()] = board.game_record().size() - 1;
}

std::string Castle::game_record_move_item(const Board&) const
{
    return ending_file == 'g' ? "O-O" : "O-O-O";
}
