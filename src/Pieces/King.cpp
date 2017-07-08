#include "Pieces/King.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Game/Color.h"
#include "Moves/Move.h"
#include "Moves/Kingside_Castle.h"
#include "Moves/Queenside_Castle.h"

King::King(Color color_in) : Piece(color_in, "K")
{
    for(int d_rank = -1; d_rank <= 1; ++d_rank)
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            if(d_rank == 0 && d_file == 0) { continue; }

            add_standard_legal_move<Move>(d_file, d_rank);
        }
    }

    add_special_legal_move<Kingside_Castle>('e', color_in == WHITE ? 1 : 8);
    add_special_legal_move<Queenside_Castle>('e', color_in == WHITE ? 1 : 8);

    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back(" \\+/ ");
    ascii_art_lines.push_back(" | | ");
    ascii_art_lines.push_back("/___\\");
    if(color() == BLACK)
    {
        ascii_art_lines[1].replace(2, 1, "#");
        ascii_art_lines[2].replace(1, 3, "###");
    }
}

bool King::is_king() const
{
    return true;
}
