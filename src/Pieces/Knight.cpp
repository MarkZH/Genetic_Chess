#include "Pieces/Knight.h"

#include <cmath>

#include "Moves/Move.h"


Knight::Knight(Color color_in) : Piece(color_in)
{
    symbol = "N";

    // possible moves
    for(int d_file = -2; d_file <= 2; ++d_file)
    {
        if(d_file == 0) { continue; }

        for(int d_rank = -2; d_rank <= 2; ++d_rank)
        {
            if(d_rank == 0 || abs(d_rank) == abs(d_file)) { continue; }

            possible_moves.emplace_back(new Move(d_file, d_rank));
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

Knight::~Knight()
{
}

bool Knight::is_knight() const
{
    return true;
}
