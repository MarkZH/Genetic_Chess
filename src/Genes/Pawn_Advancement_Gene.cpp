#include "Genes/Pawn_Advancement_Gene.h"

#include <string>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

double Pawn_Advancement_Gene::score_board(const Board& board) const
{
    double score = 0.0;
    auto perspective = board.whose_turn();
    int home_rank = (perspective == WHITE ? 2 : 7);

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->color() == perspective && piece->is_pawn())
            {
                // 1 point per move towards promotion
                score += std::abs(home_rank - rank);
            }
        }
    }

    // Count pawn promotions
    const auto& move_list = board.get_game_record();
    for(size_t i = (perspective == board.first_to_move() ? 0 : 1);
        i < move_list.size();
        i += 2)
    {
        if(move_list[i]->promotion_piece())
        {
            score += 6; // pawn made it to last rank
        }
    }

    return score/(8*6); // normalize to 8 pawns 6 ranks from home (promotion rank)
}

std::unique_ptr<Gene> Pawn_Advancement_Gene::duplicate() const
{
    return std::make_unique<Pawn_Advancement_Gene>(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}
