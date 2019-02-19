#ifndef MUTATION_RATE_GENE_H
#define MUTATION_RATE_GENE_H

#include "Gene.h"

#include <string>

class Mutation_Rate_Gene : public Gene
{
    public:
        Mutation_Rate_Gene();

        std::string name() const override;

        int mutation_count() const;

        void gene_specific_mutation() override;
        std::unique_ptr<Gene> duplicate() const override;

    protected:
        void reset_properties() const override;
        void load_properties() override;

    private:
        double mutated_components_per_mutation;

        double score_board(const Board&, const Board&, size_t) const override;
};

#endif // MUTATION_RATE_GENE_H
