#include "Genes/Pawn_Islands_Gene.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"

double Pawn_Islands_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    auto islands = 0;
    auto pawn_count = 0;
    bool on_island = false;
    auto own_pawn = Piece{perspective, Piece_Type::PAWN};

    for(char file = 'a'; file <= 'h'; ++file)
    {
        bool own_pawn_found = false;
        for(int rank = 2; rank <= 7; ++rank)
        {
            if(board.piece_on_square({file, rank}) == own_pawn)
            {
                own_pawn_found = true;
                ++pawn_count;
                if( ! on_island)
                {
                    on_island = true;
                    ++islands;
                }
            }
        }

        on_island = own_pawn_found;
    }

    if(pawn_count == 0)
    {
        return 0.0;
    }

    return (double(pawn_count)/islands)/8;
}

std::string Pawn_Islands_Gene::name() const noexcept
{
    return "Pawn Islands Gene";
}
