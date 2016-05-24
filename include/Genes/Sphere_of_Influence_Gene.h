#ifndef SPHERE_OF_INFLUENCE_GENE_H
#define SPHERE_OF_INFLUENCE_GENE_H

#include "Gene.h"

// Number of squares attacked by color weighted by proximity
// to king and strength of attacking piece.
class Sphere_of_Influence_Gene : public Gene
{
    public:
        Sphere_of_Influence_Gene(const std::shared_ptr<const Piece_Strength_Gene>& psg_in);
        virtual ~Sphere_of_Influence_Gene();

        void read_from(std::istream& is);

        Sphere_of_Influence_Gene* duplicate() const override;
        void mutate() override;

        void reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg) override;

        std::string name() const override;

    private:
        void reset_properties() const override;
        void load_properties() override;

        double score_board(const Board& board, Color color) const override;

        double legal_bonus;
        double strength_factor;
        std::shared_ptr<const Piece_Strength_Gene> piece_strength_source;
};

#endif // SPHERE_OF_INFLUENCE_GENE_H
