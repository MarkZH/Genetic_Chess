#include "Genes/Pawn_Advancement_Gene.h"

#include <string>
#include <memory>
#include <cmath>
#include <map>
#include <array>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Random.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene() : non_linearity(0.0)
{
    recompute_scores_cache();
}

double Pawn_Advancement_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    double score = 0.0;
    auto own_pawn = board.piece_instance(PAWN, perspective);
    int home_rank = (perspective == WHITE ? 2 : 7);
    int first_rank = (perspective == WHITE ? 3 : 2);
    int last_rank = (perspective == WHITE ? 7 : 6);

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = first_rank; rank <= last_rank; ++rank)
        {
            if(board.piece_on_square(file, rank) == own_pawn)
            {
                score += score_cache[std::abs(home_rank - rank)];
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
    recompute_scores_cache();
}

void Pawn_Advancement_Gene::gene_specific_mutation()
{
    non_linearity += Random::random_laplace(0.01);
    recompute_scores_cache();
}

void Pawn_Advancement_Gene::recompute_scores_cache()
{
    for(size_t i = 0; i < score_cache.size(); ++i)
    {
        score_cache[i] = std::pow(i/5.0, 1.0 + non_linearity)/8;
    }
}
