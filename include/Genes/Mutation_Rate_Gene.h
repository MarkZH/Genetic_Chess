#ifndef MUTATION_RATE_GENE_H
#define MUTATION_RATE_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Game/Color.h"

//! \brief The gene controls how much of the genome mutates per Genome::mutate() event.
class Mutation_Rate_Gene : public Clonable_Gene<Mutation_Rate_Gene>
{
    public:
        std::string name() const noexcept override;

        //! \brief Controls how many changes a call to Genome::mutate() makes to the Gene collections.
        //!
        //! \returns An integer number that determines the number of point mutations the Genome::mutate() makes.
        int mutation_count() const noexcept;

        void gene_specific_mutation() noexcept override;

    private:
        double mutated_components_per_mutation = 3.0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
        void adjust_properties(std::map<std::string, double>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, double>& properties) override;
};

#endif // MUTATION_RATE_GENE_H
