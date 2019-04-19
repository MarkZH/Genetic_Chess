#ifndef GENETIC_AI_CREATION_ERROR_H
#define GENETIC_AI_CREATION_ERROR_H

#include <stdexcept>

class Genetic_AI_Creation_Error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#endif // GENETIC_AI_CREATION_ERROR_H
