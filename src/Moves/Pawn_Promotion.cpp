#include "Moves/Pawn_Promotion.h"

#include <cctype>
#include <string>
#include <cassert>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"

Pawn_Promotion::Pawn_Promotion(const Piece* promotion_piece) :
    Pawn_Move(promotion_piece->color()),
    promote_to(promotion_piece)
{
}

void Pawn_Promotion::side_effects(Board& board, char file_start, int rank_start) const
{
    assert(rank_start == (rank_change() == 1 ? 7 : 2)); // last-minute debugging legality check

    Pawn_Move::side_effects(board, file_start, rank_start);
    board.place_piece(promote_to, file_start + file_change(),
                                  rank_start + rank_change());
}

std::string Pawn_Promotion::name() const
{
    return std::string("Pawn Promotion ") + promote_to->pgn_symbol();
}

std::string Pawn_Promotion::game_record_item(const Board& board, char file_start, int rank_start) const
{
    return Pawn_Move::game_record_item(board, file_start, rank_start) + "=" + promote_to->pgn_symbol();
}

std::string Pawn_Promotion::coordinate_move(char file_start, int rank_start) const
{
    return Move::coordinate_move(file_start, rank_start) + char(std::tolower(promote_to->fen_symbol()));
}
