#include "Utility/Math.h"

#include <cmath>
#include <cstddef>
#include <numbers>

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
    const auto Sr2 = S*std::numbers::sqrt2;
    const auto ln_x = std::log(moves_so_far + 1);

    const auto A = std::exp(M + S2/2)*(1 + std::erf((M + S2 - ln_x)/Sr2));
    const auto B = 1 + std::erf((M-ln_x)/Sr2);

    const auto expected_mean = A/B;
    return expected_mean - double(moves_so_far);
}
