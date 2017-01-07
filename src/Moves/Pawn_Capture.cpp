#include <cassert>

#include "Moves/Pawn_Capture.h"
#include "Pieces/Piece.h"
#include "Game/Board.h"
#include "Utility.h"

Pawn_Capture::Pawn_Capture(Color color_in, char dir) : Pawn_Move(color_in)
{
    if(dir == 'r')
    {
        d_file = 1;
    }
    else if(dir == 'l')
    {
        d_file = -1;
    }
    else
    {
        throw std::runtime_error(std::string("Invalid direction specification for Pawn_Capture: ")
                                + std::string().append(1, dir));
    }
}

Pawn_Capture::~Pawn_Capture()
{
}

bool Pawn_Capture::move_specific_legal(const Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int  rank_end = rank_start + rank_change();

    auto attacking_piece = board.piece_on_square(file_start, rank_start);
    auto attacked_piece  = board.piece_on_square(file_end, rank_end);
    return rank_end != (rank_change() == 1 ? 8 : 1) // not promoting (see Pawn_Promotion_by_Capture)
           && attacked_piece // must be capturing a piece
           && attacked_piece->color() != attacking_piece->color();
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
