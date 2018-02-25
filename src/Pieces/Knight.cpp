#include "Pieces/Knight.h"

#include <cmath>
#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"
#include "Game/Color.h"
#include "Moves/Move.h"


Knight::Knight(Color color_in) : Piece(color_in, "N", KNIGHT)
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

    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back(" /\") ");
    ascii_art_lines.push_back(" 7 ( ");
    ascii_art_lines.push_back("/___\\");
    if(color() == BLACK)
    {
        ascii_art_lines[1].replace(2, 1, "#");
        ascii_art_lines[2].replace(1, 3, "###");
    }
}
