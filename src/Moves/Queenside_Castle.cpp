#include "Moves/Queenside_Castle.h"

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Queenside_Castle::Queenside_Castle() : Move(-2, 0)
{
}

bool Queenside_Castle::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    return     ! board.piece_has_moved(file_start, rank_start)
            && ! board.piece_has_moved('a', rank_start)
            && ! board.king_is_in_check(board.whose_turn())
            && board.safe_for_king('d', rank_start, board.whose_turn())
            && ! board.view_piece_on_square('b', rank_start);
}

bool Queenside_Castle::can_capture() const
{
    return false;
}

void Queenside_Castle::side_effects(Board& board, char /* file_start */, int rank_start) const
{
    board.make_move('a', rank_start, 'd', rank_start); // move Rook
}

std::string Queenside_Castle::name() const
{
    return "Queenside Castle";
}

std::string Queenside_Castle::game_record_item(const Board&, char, int) const
{
    return "O-O-O";
}
