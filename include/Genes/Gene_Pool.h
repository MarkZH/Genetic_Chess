#ifndef GENE_POOL_H
#define GENE_POOL_H

#include <string>
#include <vector>

#include "Players/Genetic_AI.h"

void gene_pool(const std::string& load_file);

using Gene_Pool = std::vector<Genetic_AI>;
std::vector<Gene_Pool> load_gene_pool_file(const std::string& load_file);

#endif // GENE_POOL_H
