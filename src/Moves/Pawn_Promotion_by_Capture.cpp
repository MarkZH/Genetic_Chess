#include "Moves/Pawn_Promotion_by_Capture.h"

#include <stdexcept>

#include "Game/Piece.h"
#include "Game/Board.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Direction.h"

Pawn_Promotion_by_Capture::Pawn_Promotion_by_Capture(const Piece* promotion,
                                                     Direction dir,
                                                     char file_start) :
    Pawn_Promotion(promotion, file_start)
{
    ending_file += (dir == RIGHT ? 1 : -1);
    if( ! Board::inside_board(ending_file))
    {
        throw std::runtime_error(std::string("Invalid ending file for pawn promotion by capture: ") + ending_file);
    }

    able_to_capture = true;
}

bool Pawn_Promotion_by_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(ending_file, ending_rank); // must capture
}

std::string Pawn_Promotion_by_Capture::game_record_move_item(const Board& board) const
{
    return starting_file + std::string("x") + Pawn_Promotion::game_record_move_item(board);
}
