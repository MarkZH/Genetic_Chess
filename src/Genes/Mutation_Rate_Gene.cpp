#include "Genes/Mutation_Rate_Gene.h"

#include <string>
#include <map>

#include "Genes/Gene.h"
#include "Game/Color.h"

#include "Utility/Random.h"

class Board;

std::string Mutation_Rate_Gene::name() const noexcept
{
    return "Mutation Rate Gene";
}

int Mutation_Rate_Gene::mutation_count() const noexcept
{
    return int(mutated_components_per_mutation);
}

void Mutation_Rate_Gene::gene_specific_mutation() noexcept
{
    mutated_components_per_mutation += Random::random_laplace(1.0);
    mutated_components_per_mutation = std::max(1.0, mutated_components_per_mutation);
}

double Mutation_Rate_Gene::score_board(const Board&, Piece_Color, size_t) const noexcept
{
    return 0.0;
}

std::map<std::string, double> Mutation_Rate_Gene::list_properties() const noexcept
{
    return {{"Mutation Rate", mutated_components_per_mutation}};
}

void Mutation_Rate_Gene::load_properties(const std::map<std::string, double>& properties)
{
    mutated_components_per_mutation = properties.at("Mutation Rate");
}
