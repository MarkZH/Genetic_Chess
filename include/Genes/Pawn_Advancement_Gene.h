#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <array>

class Board;

// How close are pawns to promotion?
class Pawn_Advancement_Gene : public Gene
{
    public:
        Pawn_Advancement_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double non_linearity;
        std::array<double, 6> score_cache; // precompute scores

        double score_board(const Board& board, const Board& opposite_board, size_t) const override;
        void gene_specific_mutation() override;
        void recompute_scores_cache();
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
