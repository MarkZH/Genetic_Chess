#include "Pieces/King.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"
#include "Game/Color.h"
#include "Moves/Move.h"
#include "Moves/Castle.h"
#include "Moves/Direction.h"

King::King(Color color_in) : Piece(color_in, "K", KING)
{
    for(int d_rank = -1; d_rank <= 1; ++d_rank)
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            if(d_rank == 0 && d_file == 0) { continue; }

            add_standard_legal_move(d_file, d_rank);
        }
    }

    int base_rank = (color_in == WHITE ? 1 : 8);
    add_legal_move(std::make_unique<Castle>(base_rank, LEFT));
    add_legal_move(std::make_unique<Castle>(base_rank, RIGHT));
}
