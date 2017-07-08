#include "Moves/Pawn_Promotion_by_Capture.h"

#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Move.h"

#include <memory>

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(std::shared_ptr<const Piece> promotion,
                                                     char dir) :
    Pawn_Promotion(std::move(promotion))
{
    // 'r' and 'l' refer to the direction of the capture from white's perspective
    d_file = (dir == 'r' ? 1 : -1);
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    auto attacked_piece = board.view_piece_on_square(file_start + file_change(),
                                                     rank_start + rank_change());
    return attacked_piece && Pawn_Promotion::move_specific_legal(board, file_start, rank_start);
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
                        file_change() == 1 ? 'r' : 'l').
                        game_record_item(board, file_start, rank_start)
                            + "=" + promote_to->pgn_symbol();
}
