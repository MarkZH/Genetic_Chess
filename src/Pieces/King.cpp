#include "Pieces/King.h"

#include "Moves/Kingside_Castle.h"
#include "Moves/Queenside_Castle.h"

King::King(Color color_in) : Piece(color_in)
{
    symbol = "K";

    // possible mvoes
    for(int d_rank = -1; d_rank <= 1; ++d_rank)
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            if(d_rank == 0 && d_file == 0) { continue; }

            possible_moves.emplace_back(std::make_unique<Move>(d_file, d_rank));
        }
    }

    possible_moves.emplace_back(std::make_unique<Kingside_Castle>());
    possible_moves.emplace_back(std::make_unique<Queenside_Castle>());

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

King::~King()
{
}

bool King::is_king() const
{
    return true;
}
