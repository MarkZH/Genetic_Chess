#include "Moves/Pawn_Capture.h"

#include <cassert>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Game/Piece.h"
#include "Game/Board.h"

//! Pawn captures are identified by the color of the pawn, the direction of the capture, and the starting square.

//! \param color_in The color of the capturing pawn.
//! \param dir The direction of the move horizontally: LEFT or RIGHT.
//! \param file_start The file of the square where the move starts.
//! \param file_start The rank of the square where the move starts.
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

    if( ! Board::inside_board(ending_file))
    {
        throw std::runtime_error(std::string("Invalid pawn capture ending file: ") + ending_file);
    }

    able_to_capture = true;
}

//! This move must capture.

//! The only time a pawn is allowed to change files is while capturing.
//! This method implements that rule.
//! \param board The board state just before this move is to be made.
bool Pawn_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(ending_file, ending_rank); // must capture a piece
}

//! Pawn captures are notated by prefixing the move with the capturing pawn's starting file.

//! \param board The board state just before the move is to be made.
//! \returns The string representation of a pawn capture.
std::string Pawn_Capture::game_record_move_item(const Board& board) const
{
    return starting_file + Pawn_Move::game_record_move_item(board);
}
