#include "Genes/Pawn_Advancement_Gene.h"

#include <string>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

double Pawn_Advancement_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    const auto own_pawn = Piece{perspective, Piece_Type::PAWN};

    // Skip starting rank since those get zero score.
    const int first_rank = (perspective == Piece_Color::WHITE ? 3 : 6);
    const int last_rank = (perspective == Piece_Color::WHITE ? 7 : 2);
    const int rank_step = (perspective == Piece_Color::WHITE ? 1 : -1);

    double score = 0.0;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = first_rank; rank <= last_rank; rank += rank_step)
        {
            if(board.piece_on_square({file, rank}) == own_pawn)
            {
                score += (rank - first_rank + 1)*rank_step;
            }
        }
    }

    constexpr double max_score = 8*5;
    return score/max_score;
}

std::string Pawn_Advancement_Gene::name() const noexcept
{
    return "Pawn Advancement Gene";
}
