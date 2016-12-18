#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

// Number of squares attacked by color with bonus points for legal moves
class Sphere_of_Influence_Gene : public Gene
{
    public:
        Sphere_of_Influence_Gene();
        ~Sphere_of_Influence_Gene() override;

        Sphere_of_Influence_Gene* duplicate() const override;
        void mutate() override;

        std::string name() const override;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double legal_bonus;

        double score_board(const Board& board, Color perspective) const override;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
