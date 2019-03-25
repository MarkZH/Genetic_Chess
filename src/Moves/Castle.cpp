#include "Moves/Castle.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Game/Board.h"
#include "Game/Piece.h"

//! Construct a castling move in a certain direction.

//! \param base_rank The back rank of the player: 1 for white, 8 for black.
//! \param direction The direction of the king's move: LEFT for queenside, RIGHT for king side.
Castle::Castle(int base_rank, Direction direction) :
    Move('e',
         base_rank,
         (direction == RIGHT ? 'g' : 'c'),
         base_rank),
    rook_starting_file(direction == RIGHT ? 'h' : 'a'),
    rook_ending_file(direction == RIGHT ? 'f' : 'd')
{
    able_to_capture = false;
    is_castling_move = true;
}

//! Implements the rules for castling.

//! Namely:
//! - The king and the rook towards which the king moves have not moved during the game.
//! - The king is not in check.
//! - All of the squares between the king and rook are vacant.
//! - The king does not cross a square that is attacked by an opponent's piece.
//! \param board The board on which castling is being considered.
//! \returns Whether the current board position allows for castling.
bool Castle::move_specific_legal(const Board& board) const
{
    auto skipped_file = (starting_file + ending_file)/2;
    return     ! board.piece_has_moved(starting_file, starting_rank)
            && ! board.piece_has_moved(rook_starting_file, starting_rank)
            && ! board.king_is_in_check()
            && board.all_empty_between(starting_file, starting_rank, rook_starting_file, starting_rank)
            && board.safe_for_king(skipped_file, starting_rank, board.whose_turn());
}

//! Moves the rook to its final square.

//! This overloaded method also records when the castling move was made.
//! \param board The board on which the move is being made.
void Castle::side_effects(Board& board) const
{
    board.make_move(rook_starting_file, starting_rank, rook_ending_file, starting_rank); // move Rook
    board.castling_index[board.whose_turn()] = board.game_record().size() - 1;
}

//! Castling moves have a special notation in PGN.

//! \param board The board on which this move is made (unnecessary for this overloaded method).
//! \returns "O-O" for kingside castling or "O-O-O" for queenside castling.
std::string Castle::game_record_move_item(const Board&) const
{
    return ending_file == 'g' ? "O-O" : "O-O-O";
}
