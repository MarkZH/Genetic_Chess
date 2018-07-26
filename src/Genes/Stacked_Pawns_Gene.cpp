#include "Genes/Stacked_Pawns_Gene.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"

#include "Genes/Gene.h"

double Stacked_Pawns_Gene::score_board(const Board& board, const Board&) const
{
    double score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        int pawn_count = 0;

        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->type() == PAWN && piece->color() == board.whose_turn())
            {
                ++pawn_count;
            }
        }

        if(pawn_count > 1)
        {
            score += pawn_count - 1;
        }
    }

    return -score; // negative since, presumably, stacked pawns are bad;
                   // priority can still evolve to be negative
}

std::unique_ptr<Gene> Stacked_Pawns_Gene::duplicate() const
{
    return std::make_unique<Stacked_Pawns_Gene>(*this);
}

std::string Stacked_Pawns_Gene::name() const
{
    return "Stacked Pawns Gene";
}
