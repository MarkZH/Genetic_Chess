#include "Utility/Math.h"

#include <cmath>
#include <cstddef>

double Math::average_moves_left(const double mean_moves, const double width, const size_t moves_so_far) noexcept
{
    // Assumes the number of moves in a game has a log-normal distribution.
    //
    // A = Sum(x = moves_so_far + 1 to infinity) P(x)*x = average number of total moves
    //                                                    given that the game has already
    //                                                    progressed a number of moves equal
    //                                                    to moves_so_far
    //
    // B = Sum(x = moves_so_far + 1 to infinity) P(x) = renormalization of P(x) for a
    //                                                  truncated range
    //
    // The calculations below for A and B use integrals on continuous functions as a
    // faster approximation of the sums above.

    const auto M = std::log(mean_moves);
    const auto S = width;
    const auto S2 = std::pow(S, 2);
    const auto Sr2 = S*std::sqrt(2);
    const auto ln_x = std::log(moves_so_far);

    const auto A = std::exp(M + S2/2)*(1 + std::erf((M + S2 - ln_x)/Sr2));
    const auto B = 1 + std::erf((M-ln_x)/Sr2);

    const auto expected_mean = A/B;

    // If moves_so_far is much greater than mean_moves with a small
    // width in the log-normal distribution, A and B can end up
    // being zero, resulting in an undefined answer (NaN most likely).
    // This represents an extremely long game, so expect the game to end
    // soon.
    if( ! std::isfinite(expected_mean))
    {
        return 1.0;
    }

    return expected_mean - double(moves_so_far);
}

void Math::normalize(double& x, double& y) noexcept
{
    const auto norm = std::abs(x) + std::abs(y);
    if(norm > 0.0)
    {
        x /= norm;
        y /= norm;
    }
}

double Math::interpolate(double start_value, double end_value, double fraction) noexcept
{
    return (1.0 - fraction)*start_value + fraction*end_value;
}
