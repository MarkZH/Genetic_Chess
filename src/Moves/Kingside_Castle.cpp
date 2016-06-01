#include "Moves/Kingside_Castle.h"

#include "Game/Board.h"

Kingside_Castle::Kingside_Castle() : Move(2, 0)
{
}

bool Kingside_Castle::is_legal(const Board& board, char file_start, int rank_start) const
{
    return     ! board.piece_has_moved(file_start, rank_start)
            && ! board.piece_has_moved('h', rank_start)
            && ! board.king_is_in_check(board.whose_turn())
            && ! board.square_attacked_by('f', rank_start, opposite(board.whose_turn()))
            && ! board.square_attacked_by('g', rank_start, opposite(board.whose_turn()))
            && ! board.piece_on_square('f', rank_start)
            && ! board.piece_on_square('g', rank_start);
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
