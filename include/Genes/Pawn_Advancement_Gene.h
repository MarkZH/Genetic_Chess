#ifndef GENES_PAWN_ADVANCEMENT_GENE_H
#define GENES_PAWN_ADVANCEMENT_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <array>
#include <map>

#include "Game/Color.h"

class Board;

//! Scores a board based on how close the pawns are to promotion.
//
//! This score is modulated by an adjustment that favors the pushing
//! of either advanced pawns or rear pawns.
class Pawn_Advancement_Gene : public Gene
{
    public:
        Pawn_Advancement_Gene() noexcept;

        std::unique_ptr<Gene> duplicate() const noexcept override;

        std::string name() const noexcept override;

    protected:
        std::map<std::string, double> list_properties() const noexcept override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double non_linearity = 0.0;
        std::array<double, 6> score_cache; // precompute scores

        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const noexcept override;
        void gene_specific_mutation() noexcept override;
        void recompute_scores_cache() noexcept;
};

#endif // GENES_PAWN_ADVANCEMENT_GENE_H
