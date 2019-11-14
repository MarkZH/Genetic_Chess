#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <array>
#include <map>

#include "Game/Color.h"

class Board;

//! Scores a board based on the number of squares attacked by a side with bonus points for legality and moves close to the king.
class Sphere_of_Influence_Gene : public Gene
{
    public:
        Sphere_of_Influence_Gene() noexcept;

        std::unique_ptr<Gene> duplicate() const noexcept override;

        std::string name() const noexcept override;

    protected:
        std::map<std::string, double> list_properties() const noexcept override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double legal_square_score = 1.0;
        double illegal_square_score = 1.0;
        double king_target_factor = 0.0;

        double score_board(const Board& board, Color perspective, const size_t prior_real_moves) const noexcept override;
        void gene_specific_mutation() noexcept override;

        std::array<double, 8> scalar_cache;
        void recompute_scalar_cache() noexcept;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
