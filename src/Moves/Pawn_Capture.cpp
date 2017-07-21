#include "Moves/Pawn_Capture.h"

#include <cassert>

#include "Moves/Move.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Utility.h"

Pawn_Capture::Pawn_Capture(Color color_in, Capture_Direction dir) : Pawn_Move(color_in)
{
    assert(dir == LEFT || dir == RIGHT);

    if(dir == RIGHT)
    {
        d_file = 1;
    }
    else
    {
        d_file = -1;
    }
}

bool Pawn_Capture::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    return board.view_piece_on_square(file_start + file_change(),
                                      rank_start + rank_change()); // must capture a piece
}

bool Pawn_Capture::can_capture() const
{
    return true;
}

std::string Pawn_Capture::name() const
{
    return "Pawn Capture";
}

std::string Pawn_Capture::game_record_item(const Board&, char file_start, int rank_start) const
{
    return std::string(std::string(1, file_start))
            + "x"
            + std::string(std::string(1, char(file_start + file_change())))
            + std::string(std::to_string(rank_start + rank_change()));
}
