#include "Genes/Pawn_Advancement_Gene.h"

#include <string>
#include <memory>
#include <cmath>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Piece_Types.h"
#include "Utility.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene() : non_linearity(0.0)
{
}

double Pawn_Advancement_Gene::score_board(const Board& board, const Board&, size_t) const
{
    double score = 0.0;
    auto perspective = board.whose_turn();
    auto own_pawn = board.piece_instance(PAWN, perspective);
    int home_rank = (perspective == WHITE ? 2 : 7);

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 2; rank <= 7; ++rank)
        {
            if(board.piece_on_square(file, rank) == own_pawn)
            {
                // 1 point per move towards promotion
                score += std::pow(std::abs(home_rank - rank), 1.0 + non_linearity);
            }
        }
    }

    // normalize to 8 pawns at the far rank before promotion
    double normalizer = 8*std::pow(5.0, 1.0 + non_linearity);
    return score/normalizer;
}

std::unique_ptr<Gene> Pawn_Advancement_Gene::duplicate() const
{
    return std::make_unique<Pawn_Advancement_Gene>(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}

void Pawn_Advancement_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Non-linearity"] = non_linearity;
}

void Pawn_Advancement_Gene::load_properties()
{
    Gene::load_properties();
    non_linearity = properties["Non-linearity"];
}

void Pawn_Advancement_Gene::gene_specific_mutation()
{
    non_linearity += Random::random_laplace(0.01);
}
