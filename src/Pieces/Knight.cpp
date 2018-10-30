#include "Pieces/Knight.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"
#include "Game/Color.h"
#include "Moves/Move.h"


Knight::Knight(Color color_in) : Piece(color_in, KNIGHT)
{
    for(auto d_file : {1, 2})
    {
        auto d_rank = 3 - d_file;
        for(auto file_direction : {-1, 1})
        {
            for(auto rank_direction : {-1, 1})
            {
                add_standard_legal_move(d_file*file_direction, d_rank*rank_direction);
            }
        }
    }
}
