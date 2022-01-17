#ifndef MATH_H
#define MATH_H

#include <cstddef>

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

    //! Scale values of two numbers so their absolute values sum to one.
    //!
    //! \param[out] x First value.
    //! \param[out] y Second value.
    //!
    //! The values will be divided by std::abs(x) + std::abs(y).
    void normalize(double& x, double& y) noexcept;

    //! Scale values of three numbers so their absolute values sum to one.
    //!
    //! \param[out] x First value.
    //! \param[out] y Second value.
    //! \param[out] z Second value.
    //!
    //! The values will be divided by std::abs(x) + std::abs(y).
    void normalize(double& x, double& y, double& z) noexcept;

    //! Linearly interpolates a value.
    //!
    //! \param start_value The value returned when fraction == 0.0.
    //! \param end_value The value returned when fraction == 1.0.
    //! \param fraction A fractional value from 0.0 to 1.0 (inclusive) to indicate how close
    //!        the return value should be to one of the input values.
    double interpolate(double start_value, double end_value, double fraction) noexcept;
}

#endif // MATH_H
