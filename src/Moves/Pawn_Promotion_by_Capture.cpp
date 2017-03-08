#include "Moves/Pawn_Promotion_by_Capture.h"

#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Capture.h"

#include <memory>

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(const std::shared_ptr<const Piece>& promotion,
                                                     char dir) :
    Pawn_Promotion(promotion)
{
    d_file = (dir == 'r' ? 1 : -1);
}

Pawn_Promotion_by_Capture::~Pawn_Promotion_by_Capture()
{
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    auto attacked_piece = board.piece_on_square(file_start + file_change(), rank_start + rank_change());
    return attacked_piece && rank_start == (rank_change() == 1 ? 7 : 2); // promoting
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
