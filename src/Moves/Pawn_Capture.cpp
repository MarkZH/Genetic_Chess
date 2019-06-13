#include "Moves/Pawn_Capture.h"

#include <string>
#include <stdexcept>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Game/Piece.h"
#include "Game/Board.h"
#include "Game/Square.h"

Pawn_Capture::Pawn_Capture(Color color_in, Direction dir, Square start_in) :
    Pawn_Move(color_in, start_in)
{
    adjust_end_file(dir == RIGHT ? 1 : -1);

    if( ! end().inside_board())
    {
        throw std::invalid_argument(std::string("Invalid pawn capture starting on: ") + start().string());
    }

    able_to_capture = true;
}

bool Pawn_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(end()); // must capture a piece
}

std::string Pawn_Capture::game_record_move_item(const Board& board) const
{
    return start().file() + std::string("x") + Pawn_Move::game_record_move_item(board);
}
