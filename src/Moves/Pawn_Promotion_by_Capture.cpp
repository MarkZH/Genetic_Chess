#include "Moves/Pawn_Promotion_by_Capture.h"

#include <cassert>

#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Move.h"

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(const Piece* promotion,
                                                     Capture_Direction dir,
                                                     char file_start) :
    Pawn_Promotion(promotion, file_start)
{
    assert(dir == LEFT || dir == RIGHT);
    ending_file += (dir == RIGHT ? 1 : -1);
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board) const
{
    assert(starting_rank == (rank_change() == 1 ? 7 : 2));
    return board.view_piece_on_square(ending_file, ending_rank); // must capture
}

bool Pawn_Promotion_by_Capture::can_capture() const
{
    return true;
}

std::string Pawn_Promotion_by_Capture::game_record_move_item(const Board&) const
{
    return starting_file + std::string("x") + ending_file + std::to_string(ending_rank)
            + "=" + promote_to->pgn_symbol();
}
