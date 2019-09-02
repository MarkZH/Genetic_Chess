#ifndef MUTATION_RATE_GENE_H
#define MUTATION_RATE_GENE_H

#include "Gene.h"

#include <string>
#include <memory>
#include <map>

#include "Game/Color.h"

//! The gene controls how much of the genome mutates per Genome::mutate() event.
class Mutation_Rate_Gene : public Gene
{
    public:
        Mutation_Rate_Gene();

        std::string name() const override;

        //! Controls how many changes a call to Genome::mutate() makes to the Gene collections.
        //
        //! \returns An integer number that determines the number of point mutations the Genome::mutate() makes.
        int mutation_count() const;

        void gene_specific_mutation() override;
        std::unique_ptr<Gene> duplicate() const override;

    protected:
        std::map<std::string, double> list_properties() const override;
        void load_properties(const std::map<std::string, double>& properties) override;

    private:
        double mutated_components_per_mutation;

        double score_board(const Board& board, Color perspective, size_t prior_real_moves) const override;
};

#endif // MUTATION_RATE_GENE_H
