#ifndef MATH_H
#define MATH_H

#include <cstddef>
#include <cmath>

//! \brief Provides a collection of useful math functions.
namespace Math
{
    //! \brief Computes an estimate of the number of moves left in a game assuming a log-normal distribution.
    //!
    //! \param mean_moves The average number of moves in a game. More specifically, exp(u) where
    //!        u is the mean of the logarithm of the distribution of the number of moves.
    //! \param width The width of the distribution. More specifically, the standard deviation of the
    //!        logarithm of the distribution of the number of moves.
    //! \param moves_so_far The number of moves that have already been made in the game.
    //! \returns An estimate of the number of moves left in the game.
    //!
    //! See https://en.wikipedia.org/wiki/Log-normal_distribution for more information.
    double average_moves_left(double mean_moves, double width, size_t moves_so_far) noexcept;

    //! \brief Returns the sign of the number.
    //!
    //! \param x The input number.
    //! \returns +1 for positive numbers, -1 for negative, and 0 otherwise.
    template<typename Number>
    constexpr int sign(const Number x) noexcept
    {
        if(x > 0)
        {
            return 1;
        }

        if(x < 0)
        {
            return -1;
        }

        return 0;
    }

    //! Scale values so their absolute values sum to one.
    //!
    //! \tparam Ts Types of values.
    //! \param[out] xs All the values.
    //!
    //! Each value will be divided by std::abs(x1) + std::abs(x2) + ....
    template<typename ...Ts>
    void normalize(Ts&... xs) noexcept
    {
        // Both of the statements below are parameter pack folds.
        // The first with the plus operator (along with std::abs() on each element).
        // The second with the comma operator (along with /= sum on each element).
        // In both statements, all of the parentheses are necessary.
        const double sum = (std::abs(xs) + ...);
        ((xs /= sum), ...);
    }
}

#endif // MATH_H
