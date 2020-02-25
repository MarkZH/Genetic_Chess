#include "Genes/Stacked_Pawns_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"

double Stacked_Pawns_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    double score = 0.0;

    auto own_pawn = Piece{perspective, Piece_Type::PAWN};
    for(char file = 'a'; file <= 'h'; ++file)
    {
        int pawn_count = 0;

        for(int rank = 2; rank <= 7; ++rank)
        {
            if(board.piece_on_square({file, rank}) == own_pawn)
            {
                ++pawn_count;
            }
        }

        if(pawn_count > 1)
        {
            score += pawn_count - 1;
        }
    }

    return -score/6; // negative since, presumably, stacked pawns are bad;
                     // priority can still evolve to be negative.
                     // Divide by six since six is the maximum number of pawns
                     // that can be blocked by other pawns.
}

std::string Stacked_Pawns_Gene::name() const noexcept
{
    return "Stacked Pawns Gene";
}
