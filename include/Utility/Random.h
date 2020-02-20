#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <algorithm>
#include <cassert>
#include <string>

//! \brief A collection of functions for dealing with randomness.
namespace Random
{
    //! \brief Random number with Laplace distribution (double-sided exponential) and mean of zero
    //!
    //! \param width The inverse of the rate of drop-off as one gets further from zero.
    //! \returns A random number from a zero-centered Laplace distribution.
    //!
    //! See https://en.wikipedia.org/wiki/Laplace_distribution.
    double random_laplace(double width) noexcept;

    //! \brief Random floating point number with inclusive range from a uniform distribution.
    //!
    //! \param min The minimum number to return.
    //! \param max The maximum number to return.
    //! \returns A random number in the range [min, max].
    double random_real(double min, double max) noexcept;

    //! \brief Random integral number with inclusive range from a uniform distribution.
    //!
    //! \param min The minimum number to return.
    //! \param max The maximum number to return.
    //! \returns A random number in the range [min, max].
    template<typename Integer>
    Integer random_integer(Integer min, Integer max) noexcept
    {
        thread_local static std::mt19937_64 generator(std::random_device{}());
        using uid = std::uniform_int_distribution<Integer>;
        thread_local static auto dist = uid{};
        return dist(generator, typename uid::param_type{min, max});
    }

    //! \brief Random 64-bit unsigned integer.
    //!
    //! \returns a random 64-bit integer with uniform probability.
    uint64_t random_unsigned_int64() noexcept;

    //! \brief Simulate a fair coin flip.
    //!
    //! \returns true with probability 50%
    bool coin_flip() noexcept;

    //! \brief Simulate a fair trial.
    //!
    //! \param probability The probability of success.
    //! \returns true with given probability
    bool success_probability(double probability) noexcept;

    //! \brief Shuffles the order of a list.
    //!
    //! \param[out] list A sequential collection of items whose order will be shuffled after the call.
    template<class List>
    void shuffle(List& list) noexcept
    {
        thread_local static std::mt19937_64 generator(std::random_device{}());
        std::shuffle(list.begin(), list.end(), generator);
    }

    //! \brief Select random element from random-access container.
    //!
    //! \param container A collection of items that allows for access by an index.
    template<typename Container>
    typename Container::const_reference random_element(const Container& container) noexcept
    {
        assert( ! container.empty());
        return container.at(random_integer(size_t{0}, container.size() - 1));
    }

    //! \brief Select random element from random-access container.
    //!
    //! \param container A collection of items that allows for access by an index.
    template<typename Container>
    typename Container::reference random_element(Container& container) noexcept
    {
        assert( ! container.empty());
        return container[random_integer(size_t{0}, container.size() - 1)];
    }

    //! \brief Create a string of random lowercase letters.
    //!
    //! \param size The desired size of the random string.
    std::string random_string(size_t size) noexcept;
}

#endif // RANDOM_H
