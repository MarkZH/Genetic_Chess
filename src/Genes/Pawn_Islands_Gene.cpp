#include "Genes/Pawn_Islands_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"

double Pawn_Islands_Gene::score_board(const Board& board, const Piece_Color perspective, size_t, double) const noexcept
{
    auto islands = 0;
    bool on_island = false;
    const auto own_pawn = Piece{perspective, Piece_Type::PAWN};

    for(char file = 'a'; file <= 'h'; ++file)
    {
        bool own_pawn_found = false;
        for(int rank = 2; rank <= 7; ++rank)
        {
            if(board.piece_on_square({file, rank}) == own_pawn)
            {
                own_pawn_found = true;
                if( ! on_island)
                {
                    ++islands;
                }
                break;
            }
        }

        on_island = own_pawn_found;
    }

    return -islands/4.0;
}

std::string Pawn_Islands_Gene::name() const noexcept
{
    return "Pawn Islands Gene";
}
