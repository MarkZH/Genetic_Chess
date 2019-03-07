#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <algorithm>

namespace Random
{
    // Random number with Laplace distribution (double-sided exponential) and mean of zero
    double random_laplace(double width);

    // Random number with inclusive range
    double random_real(double min, double max);
    int random_integer(int min, int max);
    uint64_t random_unsigned_int64();

    // Return true with probability 50%
    bool coin_flip();

    // Return true with given probability
    bool success_probability(double probability);

    // Shuffles the order of the list
    template<class List>
    void shuffle(List& list)
    {
        thread_local static std::mt19937_64 generator(std::random_device{}());
        std::shuffle(list.begin(), list.end(), generator);
    }
}

#endif // RANDOM_H
