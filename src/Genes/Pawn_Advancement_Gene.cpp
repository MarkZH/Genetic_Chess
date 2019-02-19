#include "Genes/Pawn_Advancement_Gene.h"

#include <string>
#include <memory>
#include <cmath>
#include <map>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Piece.h"
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
                score += std::pow(std::abs(home_rank - rank)/5.0, 1.0 + non_linearity)/8;
            }
        }
    }

    return score;
}

std::unique_ptr<Gene> Pawn_Advancement_Gene::duplicate() const
{
    return std::make_unique<Pawn_Advancement_Gene>(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}

std::map<std::string, double> Pawn_Advancement_Gene::list_properties() const
{
    auto properties = Gene::list_properties();
    properties["Non-linearity"] = non_linearity;
    return properties;
}

void Pawn_Advancement_Gene::load_properties(const std::map<std::string, double>& properties)
{
    Gene::load_properties(properties);
    non_linearity = properties.at("Non-linearity");
}

void Pawn_Advancement_Gene::gene_specific_mutation()
{
    non_linearity += Random::random_laplace(0.01);
}
