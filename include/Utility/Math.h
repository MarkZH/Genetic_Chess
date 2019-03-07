#ifndef MATH_H
#define MATH_H

#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace Math
{
    const auto win_score = std::numeric_limits<double>::infinity();
    const auto lose_score = -win_score;

    double average_moves_left(double mean_moves, double width, size_t moves_so_far);

    template<typename Number>
    Number clamp(Number n, Number low, Number high)
    {
        return std::min(std::max(n, low), high);
    }

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

    template<typename Number>
    Number lower_bound_reflect(Number x, Number low)
    {
        return low + std::abs(x - low);
    }

    template<typename Number>
    Number upper_bound_reflect(Number x, Number high)
    {
        return high - std::abs(high - x);
    }

    template<typename Number>
    Number reflect(Number x, Number low, Number high)
    {
        if(low > high)
        {
            throw std::runtime_error("Bad limits in reflect(): lower bound (" + std::to_string(low) + ") > upper bound (" + std::to_string(high) + ")");
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
