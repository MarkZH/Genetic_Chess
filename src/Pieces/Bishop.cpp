#include "Pieces/Bishop.h"
#include "Pieces/Piece.h"
#include "Game/Color.h"

Bishop::Bishop(Color color_in) : Piece(color_in)
{
    symbol = "B";

    // possible moves
    for(int d_rank : {-1, 1})
    {
        for(int d_file : {-1, 1})
        {
            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                possible_moves.emplace_back(std::make_unique<Move>(move_size*d_file, move_size*d_rank));
            }
        }
    }

    // ASCII art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back(" (V) ");
    ascii_art_lines.push_back(" ) ( ");
    ascii_art_lines.push_back("/___\\");
    if(color() == BLACK)
    {
        ascii_art_lines[1].replace(2, 1, "#");
        ascii_art_lines[2].replace(1, 3, "###");
    }
}

Bishop::~Bishop()
{
}

bool Bishop::is_bishop() const
{
    return true;
}
