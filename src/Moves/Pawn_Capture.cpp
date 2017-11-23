#include "Moves/Pawn_Capture.h"

#include <cassert>

#include "Moves/Move.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Utility.h"

Pawn_Capture::Pawn_Capture(Color color_in, Capture_Direction dir, char file_start, int rank_start) :
    Pawn_Move(color_in, file_start, rank_start)
{
    assert(dir == LEFT || dir == RIGHT);

    if(dir == RIGHT)
    {
        ending_file += 1;
    }
    else
    {
        ending_file -= 1;
    }
}

bool Pawn_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(ending_file, ending_rank); // must capture a piece
}

bool Pawn_Capture::can_capture() const
{
    return true;
}

std::string Pawn_Capture::game_record_move_item(const Board&) const
{
    return starting_file
            + std::string("x")
            + ending_file
            + std::to_string(ending_rank);
}
