#ifndef MATH_H
#define MATH_H

#include <limits>
#include <algorithm>
#include <cassert>

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
        assert(low <= high);
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
}

#endif // MATH_H
