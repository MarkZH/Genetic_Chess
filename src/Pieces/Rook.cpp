#include "Pieces/Rook.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"

Rook::Rook(Color color_in) : Piece(color_in, "R", ROOK)
{
    for(int d_file = -1; d_file <= 1; ++d_file)
    {
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            if(d_file == 0 && d_rank == 0) { continue; }
            if(d_file != 0 && d_rank != 0) { continue; }

            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}
