#include "Moves/Pawn_Move.h"
#include "Moves/Move.h"
#include "Game/Board.h"

Pawn_Move::Pawn_Move(Color color_in) : Move(0, (color_in == WHITE ? 1 : -1))
{
}

Pawn_Move::~Pawn_Move()
{
}

bool Pawn_Move::is_legal(const Board& board, char file_start, int rank_start) const
{
    return rank_start != (rank_change() == 1 ? 7 : 2)
        && ! board.piece_on_square(file_start, rank_start + rank_change());
}

void Pawn_Move::side_effects(Board& board, char /* file_start */, int /* rank_start */) const
{
    board.repeat_count.clear();
}

std::string Pawn_Move::name() const
{
    return "Pawn Move";
}

std::string Pawn_Move::game_record_item(const Board& board, char file_start, int rank_start) const
{
    return Move::game_record_item(board, file_start, rank_start);
}
