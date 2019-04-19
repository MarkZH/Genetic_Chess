#ifndef MATH_H
#define MATH_H

#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>

//! Provides a collection of useful math functions.
namespace Math
{
    //! The score assigned to a checkmate board position for the winning Minimax_AI.
    const auto win_score = std::numeric_limits<double>::infinity();
    //! The score assigned to a checkmate board position for the losing Minimax_AI.
    const auto lose_score = -win_score;

    //! Computes an estimate of the number of moves left in a game assuming a log-normal distribution.

    //! \param mean_moves The average number of moves in a game. More specifically, exp(u) where
    //!        u is the mean of the logarithm of the distribution of the number of moves.
    //! \param width The width of the distribution. More specifically, the standard deviation of the
    //!        logarithm of the distribution of the number of moves.
    //! \param moves_so_far The number of moves that have already been made in the game.
    //! \returns An estimate of the number of moves left in the game.
    //!
    //! See https://en.wikipedia.org/wiki/Log-normal_distribution for more information.
    double average_moves_left(double mean_moves, double width, size_t moves_so_far);

    //! Clamps a number to a given range.

    //! \param n The input number for the range check.
    //! \param low The lower bound of the range.
    //! \param high The upper bound of the range.
    //! \returns The number n if it lies between the bounds (inclusive), otherwise it returns
    //!          the nearest bound (upper or lower).
    template<typename Number>
    Number clamp(Number n, Number low, Number high)
    {
        return std::min(std::max(n, low), high);
    }

    //! Returns the sign of the number.

    //! \param x The input nuber.
    //! \returns +1 for positive numbers, -1 for negative, and 0 otherwise.
    template<typename Number>
    int sign(Number x)
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

    //! If a number is lower than the bound, reflect the number to the other side of the bound.

    //! \param x The input number.
    //! \param low The lower bound.
    //! \returns A number that is the same distance from the lower bound, but on the correct side.
    template<typename Number>
    Number lower_bound_reflect(Number x, Number low)
    {
        return low + std::abs(x - low);
    }

    //! If a number is higher than the bound, reflect the number to the other side of the bound.

    //! \param x The input number.
    //! \param high The upper bound.
    //! \returns A number that is the same distance from the upper bound, but on the correct side.
    template<typename Number>
    Number upper_bound_reflect(Number x, Number high)
    {
        return high - std::abs(high - x);
    }

    //! Repeatedly calls upper_bound_reflect() and lower_bound_reflect() until the number is in the correct range.

    //! \param x The input number.
    //! \param low The lower bound.
    //! \param high The upper bound.
    //! \returns A number between the upper and lower bound.
    template<typename Number>
    Number reflect(Number x, Number low, Number high)
    {
        if(low > high)
        {
            throw std::invalid_argument("Bad limits in reflect(): lower bound (" + std::to_string(low) + ") > upper bound (" + std::to_string(high) + ")");
        }

        if(low < high)
        {
            while(x < low || x > high)
            {
                x = upper_bound_reflect(lower_bound_reflect(x, low), high);
            }

            return x;
        }

        // low == high
        return low;
    }
}

#endif // MATH_H
