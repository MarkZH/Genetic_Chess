#include "Moves/Pawn_Capture.h"

#include <string>
#include <stdexcept>

#include "Moves/Pawn_Move.h"
#include "Moves/Direction.h"
#include "Game/Piece.h"
#include "Game/Board.h"
#include "Game/Square.h"

//! Pawn captures are identified by the color of the pawn, the direction of the capture, and the starting square.
//
//! \param color_in The color of the capturing pawn.
//! \param dir The direction of the move horizontally: LEFT or RIGHT.
//! \param file_start The file of the square where the move starts.
//! \param rank_start The rank of the square where the move starts.
Pawn_Capture::Pawn_Capture(Color color_in, Direction dir, char file_start, int rank_start) :
    Pawn_Move(color_in, file_start, rank_start)
{
    adjust_end_file(dir == RIGHT ? 1 : -1);

    if( ! end().inside_board())
    {
        throw std::invalid_argument(std::string("Invalid pawn capture starting on: ") + start().string());
    }

    able_to_capture = true;
}

//! This move must capture.
//
//! \param board The board state just before this move is to be made.
bool Pawn_Capture::move_specific_legal(const Board& board) const
{
    return board.piece_on_square(end()); // must capture a piece
}

//! Pawn captures are notated by prefixing the move with the capturing pawn's starting file.
//
//! \param board The board state just before the move is to be made.
//! \returns The string representation of a pawn capture.
std::string Pawn_Capture::game_record_move_item(const Board& board) const
{
    return start().file() + std::string("x") + Pawn_Move::game_record_move_item(board);
}
