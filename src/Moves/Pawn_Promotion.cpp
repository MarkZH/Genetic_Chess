#include "Moves/Pawn_Promotion.h"

#include <cctype>
#include <string>
#include <cassert>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Pawn_Promotion::Pawn_Promotion(const Piece* promotion_piece, char file_start) :
    Pawn_Move(promotion_piece->color(), file_start, promotion_piece->color() == WHITE ? 7 : 2),
    promote_to(promotion_piece)
{
}

void Pawn_Promotion::side_effects(Board& board) const
{
    assert(starting_rank == (rank_change() == 1 ? 7 : 2)); // last-minute debugging legality check

    Pawn_Move::side_effects(board);
    board.place_piece(promotion_piece(), ending_file, ending_rank);
}

std::string Pawn_Promotion::game_record_move_item(const Board& board) const
{
    return Pawn_Move::game_record_move_item(board) + "=" + promote_to->pgn_symbol();
}

std::string Pawn_Promotion::coordinate_move() const
{
    return Move::coordinate_move() + char(std::tolower(promote_to->fen_symbol()));
}

char Pawn_Promotion::promotion_piece_symbol() const
{
    return promote_to->pgn_symbol().front();
}

const Piece* Pawn_Promotion::promotion_piece() const
{
    return promote_to;
}
