#ifndef GENE_POOL_H
#define GENE_POOL_H

#include <string>

//! \file

//! \brief Run a gene pool according to a configuration text file.
//!
//! \param config_file The name of the file with the gene pool configuration data.
[[noreturn]] void gene_pool(const std::string& config_file);

#endif // GENE_POOL_H
