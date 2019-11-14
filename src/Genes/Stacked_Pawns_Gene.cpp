#include "Genes/Stacked_Pawns_Gene.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"

double Stacked_Pawns_Gene::score_board(const Board& board, Color perspective, size_t) const noexcept
{
    double score = 0.0;

    auto own_pawn = Piece{perspective, PAWN};
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

std::unique_ptr<Gene> Stacked_Pawns_Gene::duplicate() const noexcept
{
    return std::make_unique<Stacked_Pawns_Gene>(*this);
}

std::string Stacked_Pawns_Gene::name() const noexcept
{
    return "Stacked Pawns Gene";
}
