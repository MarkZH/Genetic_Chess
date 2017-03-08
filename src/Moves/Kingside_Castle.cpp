#include "Moves/Kingside_Castle.h"

#include "Game/Board.h"

Kingside_Castle::Kingside_Castle() : Move(2, 0)
{
}

Kingside_Castle::~Kingside_Castle()
{
}

bool Kingside_Castle::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    return     ! board.piece_has_moved(file_start, rank_start)
            && ! board.piece_has_moved('h', rank_start)
            && ! board.king_is_in_check(board.whose_turn())
            && board.safe_for_king('f', rank_start, board.whose_turn());
}

bool Kingside_Castle::can_capture() const
{
    return false;
}

void Kingside_Castle::side_effects(Board& board, char /* file_start */, int rank_start) const
{
    board.make_move('h', rank_start, 'f', rank_start); // move Rook
}

std::string Kingside_Castle::name() const
{
    return "Kingside Castle";
}

std::string Kingside_Castle::game_record_item(const Board&, char, int) const
{
    return "O-O";
}
