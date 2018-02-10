#include "Pieces/Rook.h"

#include "Pieces/Piece.h"
#include "Moves/Move.h"

Rook::Rook(Color color_in) : Piece(color_in, "R")
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

    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back(" |U| ");
    ascii_art_lines.push_back(" | | ");
    ascii_art_lines.push_back("/___\\");
    if(color() == BLACK)
    {
        ascii_art_lines[1].replace(2, 1, "#");
        ascii_art_lines[2].replace(1, 3, "###");
    }
}

bool Rook::is_rook() const
{
    return true;
}
