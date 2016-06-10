#include "Genes/Pawn_Advancement_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Genes/Piece_Strength_Gene.h"
#include "Utility.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source_in) :
    Gene(0.0),
    piece_strength_source(piece_strength_source_in),
    promoted_pawn_bonus(0.0)
{
}

void Pawn_Advancement_Gene::reset_properties() const
{
    reset_base_properties();
    properties["Promoted Pawn Bonus"] = promoted_pawn_bonus;
}

void Pawn_Advancement_Gene::load_properties()
{
    load_base_properties();
    promoted_pawn_bonus = properties["Promoted Pawn Bonus"];
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

void Pawn_Advancement_Gene::mutate()
{
    Gene::mutate();
    promoted_pawn_bonus += Random::random_normal(1.0);
    promoted_pawn_bonus = std::max(promoted_pawn_bonus, 0.0);
}

void Pawn_Advancement_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strength_source = psg;
}
