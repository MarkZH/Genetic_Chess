#include "Moves/Pawn_Capture.h"

#include <cassert>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"

Pawn_Capture::Pawn_Capture(Color color_in, Direction dir, char file_start, int rank_start) :
    Pawn_Move(color_in, file_start, rank_start)
{
    if(dir == RIGHT)
    {
        ending_file += 1;
    }
    else
    {
        ending_file -= 1;
    }

    able_to_capture = true;
}

bool Pawn_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(ending_file, ending_rank); // must capture a piece
}

std::string Pawn_Capture::game_record_move_item(const Board&) const
{
    return starting_file
            + std::string("x")
            + ending_file
            + std::to_string(ending_rank);
}
