#include "Genes/Pawn_Advancement_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene() : Gene(0.0)
{
}

Pawn_Advancement_Gene::~Pawn_Advancement_Gene()
{
}

double Pawn_Advancement_Gene::score_board(const Board& board, Color perspective) const
{
    double score = 0.0;
    int home_rank = (perspective == WHITE ? 2 : 7);

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->color() == perspective && toupper(piece->fen_symbol()) == 'P')
            {
                // 1 point per pawn + 1 point per move towards promotion
                score += std::abs(home_rank - rank);
            }
        }
    }

    return score/(8.*5.); // normalize to 8 pawns 5 ranks from home (just before promotion)
}

Pawn_Advancement_Gene* Pawn_Advancement_Gene::duplicate() const
{
    return new Pawn_Advancement_Gene(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}
