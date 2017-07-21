#include "Moves/Pawn_Promotion_by_Capture.h"

#include <cassert>

#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Move.h"

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(const Piece* promotion,
                                                     Capture_Direction dir) :
    Pawn_Promotion(promotion)
{
    assert(dir == LEFT || dir == RIGHT);
    d_file = (dir == RIGHT ? 1 : -1);
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    assert(rank_start == (rank_change() == 1 ? 7 : 2));
    return board.view_piece_on_square(file_start + file_change(),
                                      rank_start + rank_change()); // must capture
}

bool Pawn_Promotion_by_Capture::can_capture() const
{
    return true;
}

std::string Pawn_Promotion_by_Capture::name() const
{
    return "Pawn Promotion by Capture " + promote_to->pgn_symbol();
}

std::string Pawn_Promotion_by_Capture::game_record_item(const Board& board, char file_start, int rank_start) const
{
    return Pawn_Capture(rank_change() == 1 ? WHITE : BLACK,
                        file_change() == 1 ? RIGHT : LEFT).
                        game_record_item(board, file_start, rank_start)
                            + "=" + promote_to->pgn_symbol();
}
