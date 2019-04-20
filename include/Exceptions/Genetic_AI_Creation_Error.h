#ifndef GENETIC_AI_CREATION_ERROR_H
#define GENETIC_AI_CREATION_ERROR_H

#include <stdexcept>

//! An exception thrown when a Genetic_AI cannot be successfully constructed.
class Genetic_AI_Creation_Error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#endif // GENETIC_AI_CREATION_ERROR_H
