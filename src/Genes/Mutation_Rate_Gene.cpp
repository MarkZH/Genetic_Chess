#include "Genes/Mutation_Rate_Gene.h"

#include <string>
#include <memory>
#include <cmath>

#include "Genes/Gene.h"
#include "Utility.h"

class Board;

Mutation_Rate_Gene::Mutation_Rate_Gene() : mutated_components_per_mutation(10)
{
}

std::string Mutation_Rate_Gene::name() const
{
    return "Mutation Rate Gene";
}

int Mutation_Rate_Gene::mutation_count() const
{
    // THe larger the fractional component of the mutated_components_per_mutation,
    // the greater the probability of return an extra mutation count.
    return int(mutated_components_per_mutation + Random::random_real(0.0, 1.0));
}

void Mutation_Rate_Gene::gene_specific_mutation()
{
    mutated_components_per_mutation += Random::random_laplace(1.0);
    mutated_components_per_mutation = 1.0 + std::abs(mutated_components_per_mutation - 1.0);
}

std::unique_ptr<Gene> Mutation_Rate_Gene::duplicate() const
{
    return std::make_unique<Mutation_Rate_Gene>(*this);
}

double Mutation_Rate_Gene::score_board(const Board&, const Board&, size_t) const
{
    return 0.0;
}

std::map<std::string, double> Mutation_Rate_Gene::list_properties() const
{
    return {{"Mutation Rate", mutated_components_per_mutation}};
}

void Mutation_Rate_Gene::load_properties(const std::map<std::string, double>& properties)
{
    mutated_components_per_mutation = properties.at("Mutation Rate");
}
