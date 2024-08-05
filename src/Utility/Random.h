#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <algorithm>
#include <cassert>
#include <string>
#include <type_traits>

//! \brief A collection of functions for dealing with randomness.
namespace Random
{
    //! \brief Specifying the type of random number generator used.
    using Random_Bits_Generator = std::mt19937_64;

    //! Creates and returns a randomly seeded random bit generator.
    Random_Bits_Generator get_new_seeded_random_bit_source() noexcept;

    //! \brief Random number with Laplace distribution (double-sided exponential) and mean of zero
    //!
    //! \param width The inverse of the rate of drop-off as one gets further from zero.
    //! \returns A random number from a zero-centered Laplace distribution.
    //!
    //! See https://en.wikipedia.org/wiki/Laplace_distribution.
    double random_laplace(double width) noexcept;

    //! \brief Random integral number with inclusive range from a uniform distribution.
    //!
    //! \param min The minimum number to return.
    //! \param max The maximum number to return.
    //! \returns A random number in the range [min, max].
    template<typename Integer> requires std::is_integral_v<Integer>
    Integer random_integer(const Integer min, const Integer max) noexcept
    {
        thread_local static auto generator = get_new_seeded_random_bit_source();
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

    //! \brief Simulate a fair trial with a rational probability of success.
    //!
    //! To be specific, \code{cpp}success_probability(3, 4)\endcode returns true with probability 3/4.
    //!
    //! \param successes The number of expected successes in a given number of attempts.
    //! \param attempts The given number of attempts.
    //! \returns true with given probability (successes/attempts).
    bool success_probability(size_t successes, size_t attempts) noexcept;

    //! \brief Select random element from random-access container.
    //!
    //! \param container A collection of items that allows for access by an index.
    template<typename Container>
    typename Container::const_reference random_element(const Container& container) noexcept
    {
        assert( ! container.empty());
        return container.at(random_integer({0}, container.size() - 1));
    }

    //! \brief Select random element from random-access container.
    //!
    //! \param container A collection of items that allows for access by an index.
    template<typename Container>
    typename Container::reference random_element(Container& container) noexcept
    {
        assert( ! container.empty());
        return container[random_integer({0}, container.size() - 1)];
    }

    //! \brief Create a string of random lowercase letters.
    //!
    //! \param size The desired size of the random string.
    std::string random_string(size_t size) noexcept;

    //! \brief Completely randomly shuffle a list so that any order is equally likely.
    //! \tparam Container Any ordered container type.
    //! \param container The container to be shuffled.
    template<typename Container>
    void shuffle(Container& container) noexcept
    {
        thread_local static auto generator = get_new_seeded_random_bit_source();
        std::shuffle(container.begin(), container.end(), generator);
    }
}

#endif // RANDOM_H
