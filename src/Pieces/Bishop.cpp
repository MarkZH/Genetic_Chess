#include "Pieces/Bishop.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"
#include "Game/Color.h"

Bishop::Bishop(Color color_in) : Piece(color_in, "B", BISHOP)
{
    for(int d_rank : {-1, 1})
    {
        for(int d_file : {-1, 1})
        {
            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}
