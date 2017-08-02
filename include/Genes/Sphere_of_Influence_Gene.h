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
        void reset_properties() const override;
        void load_properties() override;

    private:
        double legal_bonus;
        double king_target_factor;

        double score_board(const Board& board) const override;
        void gene_specific_mutation() override;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
