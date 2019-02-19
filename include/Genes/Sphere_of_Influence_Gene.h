#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>

class Board;

// Number of squares attacked by color with bonus points for legal moves
// and moves close to king
class Sphere_of_Influence_Gene : public Gene
{
    public:
        Sphere_of_Influence_Gene();

        std::unique_ptr<Gene> duplicate() const override;

        std::string name() const override;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double legal_square_score;
        double illegal_square_score;
        double king_target_factor;

        double score_board(const Board& board, const Board& opposite_board, size_t depth) const override;
        void gene_specific_mutation() override;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
