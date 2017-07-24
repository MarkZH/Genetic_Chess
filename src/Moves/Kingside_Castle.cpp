#include "Moves/Kingside_Castle.h"

#include "Moves/Move.h"
#include "Game/Board.h"

Kingside_Castle::Kingside_Castle(int base_rank) : Move('e', base_rank,
                                                       'g', base_rank)
{
}

bool Kingside_Castle::move_specific_legal(const Board& board) const
{
    return     ! board.piece_has_moved(starting_file, starting_rank)
            && ! board.piece_has_moved('h', starting_rank)
            && ! board.king_is_in_check(board.whose_turn())
            && board.safe_for_king('f', starting_rank, board.whose_turn());
}

bool Kingside_Castle::can_capture() const
{
    return false;
}

void Kingside_Castle::side_effects(Board& board) const
{
    board.make_move('h', starting_rank, 'f', starting_rank); // move Rook
}

std::string Kingside_Castle::game_record_move_item(const Board&) const
{
    return "O-O";
}
