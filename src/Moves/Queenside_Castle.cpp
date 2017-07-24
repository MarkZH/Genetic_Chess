#include "Moves/Queenside_Castle.h"

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Queenside_Castle::Queenside_Castle(int base_rank) : Move('e', base_rank, 'c', base_rank)
{
}

bool Queenside_Castle::move_specific_legal(const Board& board) const
{
    return     ! board.piece_has_moved(starting_file, starting_rank)
            && ! board.piece_has_moved('a', starting_rank)
            && ! board.view_piece_on_square('b', starting_rank)
            && ! board.king_is_in_check(board.whose_turn())
            && board.safe_for_king('d', starting_rank, board.whose_turn());
}

bool Queenside_Castle::can_capture() const
{
    return false;
}

void Queenside_Castle::side_effects(Board& board) const
{
    board.make_move('a', starting_rank, 'd', starting_rank); // move Rook
}

std::string Queenside_Castle::game_record_move_item(const Board&) const
{
    return "O-O-O";
}
