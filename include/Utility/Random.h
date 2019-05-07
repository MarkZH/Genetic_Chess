#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <algorithm>

//! A collection of functions for dealing with randomness.
namespace Random
{
    //! Random number with Laplace distribution (double-sided exponential) and mean of zero
    //
    //! \param width The inverse of the rate of drop-off as one gets further from zero.
    //! \returns A random number from a zero-centered Laplace distribution.
    //!
    //! See https://en.wikipedia.org/wiki/Laplace_distribution.
    double random_laplace(double width);

    //! Random floating point number with inclusive range from a uniform distribution.
    //
    //! \param min The minimum number to return.
    //! \param max The maximum number to return.
    //! \returns A random number in the range [min, max].
    double random_real(double min, double max);

    //! Random integral number with inclusive range from a uniform distribution.
    //
    //! \param min The minimum number to return.
    //! \param max The maximum number to return.
    //! \returns A random number in the range [min, max].
    int random_integer(int min, int max);

    //! Random unsigned integer with inclusive range from a uniform distribution.
    //
    //! \param max The maximum number to return.
    //! \returns A random number in the range [0, max].
    size_t random_index(size_t max);

    //! Random 64-bit unsigned integer.
    //
    //! \returns a random 64-bit integer with uniform probability.
    uint64_t random_unsigned_int64();

    //! Simulate a fair coin flip.
    //
    //! \returns true with probability 50%
    bool coin_flip();

    // Simulate a fair trial.

    //! \param probability The probability of success.
    //! \returns true with given probability
    bool success_probability(double probability);

    //! Shuffles the order of a list.
    //
    //! \param[out] list A sequential collection of items whose order will be shuffled after the call.
    template<class List>
    void shuffle(List& list)
    {
        thread_local static std::mt19937_64 generator(std::random_device{}());
        std::shuffle(list.begin(), list.end(), generator);
    }

    //! Select random element from random-access container.
    //
    //! \param container A collection of items that allows for access by an index.
    template<typename Container>
    typename Container::const_reference random_element(const Container& container)
    {
        return container.at(random_index(container.size() - 1));
    }
}

#endif // RANDOM_H
