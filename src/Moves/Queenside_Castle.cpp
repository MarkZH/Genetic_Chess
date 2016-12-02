#include "Moves/Queenside_Castle.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Queenside_Castle::Queenside_Castle() : Move(-2, 0)
{
}

Queenside_Castle::~Queenside_Castle()
{
}

bool Queenside_Castle::is_legal(const Board& board, char file_start, int rank_start, bool king_check) const
{
    return     ! board.piece_has_moved(file_start, rank_start)
            && ! board.piece_has_moved('a', rank_start)
            && ! board.king_is_in_check(board.whose_turn())
            && ! board.square_attacked_by('c', rank_start, opposite(board.whose_turn()))
            && ! board.square_attacked_by('d', rank_start, opposite(board.whose_turn()))
            && ! board.piece_on_square('b', rank_start)
            && ! board.piece_on_square('c', rank_start)
            && ! board.piece_on_square('d', rank_start)
            && Move::is_legal(board, file_start, rank_start, king_check);
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
