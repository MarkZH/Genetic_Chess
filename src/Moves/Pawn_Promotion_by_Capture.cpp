#include "Moves/Pawn_Promotion_by_Capture.h"

#include <stdexcept>

#include "Game/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Direction.h"

//! Create a pawn capture that also promotes.

//! \param promotion A pointer to the piece that the pawn will promote to.
//! \param dir The direction of the capture.
//! \param file_start The horizontal direction of the capture.
Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(Piece_Type promotion,
                                                     Color color,
                                                     Direction dir,
                                                     char file_start) :
    Pawn_Promotion(promotion, color, file_start)
{
    adjust_end_file(dir == RIGHT ? 1 : -1);
    if( ! Board::inside_board(end_file()))
    {
        throw std::runtime_error(std::string("Invalid ending file for pawn promotion by capture: ") + end_file());
    }

    able_to_capture = true;
}

//! This move must capture.

//! \param board The board state just before the move.
//! \returns Whether there is an opposing piece to capture.
bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(end_file(), end_rank()); // must capture
}

//! Combine pawn capture note with a promotion note.

//! \param board The board state just before the move.
//! \returns A textual record of a capture and a promotion.
std::string Pawn_Promotion_by_Capture::game_record_move_item(const Board& board) const
{
    return start_file() + std::string("x") + Pawn_Promotion::game_record_move_item(board);
}
