#include "Genes/Pawn_Advancement_Gene.h"

#include <string>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"

double Pawn_Advancement_Gene::score_board(const Board& board, const Board&, size_t) const
{
    double score = 0.0;
    auto perspective = board.whose_turn();
    auto own_pawn = board.get_piece(PAWN, perspective);
    int home_rank = (perspective == WHITE ? 2 : 7);

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 2; rank <= 7; ++rank)
        {
            if(board.piece_on_square(file, rank) == own_pawn)
            {
                // 1 point per move towards promotion
                score += std::abs(home_rank - rank);
            }
        }
    }

    return score/(8*5); // normalize to 8 pawns at the far rank (i.e., promoted)
}

std::unique_ptr<Gene> Pawn_Advancement_Gene::duplicate() const
{
    return std::make_unique<Pawn_Advancement_Gene>(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}
