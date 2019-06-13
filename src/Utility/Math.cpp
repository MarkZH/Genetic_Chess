#include "Utility/Math.h"

#include <cmath>
#include <cstddef>

double Math::average_moves_left(double mean_moves, double width, size_t moves_so_far)
{
    // Assumes the number of moves in a game has a log-normal distribution.
    //
    // A = Sum(x = moves_so_far + 1 to infinity) P(x)*x = average number of moves
    //                                                    given game has already progressed
    //                                                    moves_so_far
    //
    // B = Sum(x = moves_so_far + 1 to infinity) P(x) = renormalization of P(x) for a
    //                                                  truncated range
    //
    // The calculations below for A and B use integrals on continuous functions as a
    // faster approximation.

    auto M = std::log(mean_moves);
    auto S = width;
    auto S2 = std::pow(S, 2);
    auto Sr2 = S*std::sqrt(2);
    auto ln_x = std::log(moves_so_far);

    auto A = std::exp(M + S2/2)*(1 + std::erf((M + S2 - ln_x)/Sr2));
    auto B = 1 + std::erf((M-ln_x)/Sr2);

    auto expected_mean = A/B;

    // If moves_so_far is much greater than mean_moves with a small
    // width in the log-normal distribution, A and B can end up
    // being zero, resulting in an undefined answer (NaN most likely).
    // This represents an extremely long game, so expect the game to end
    // soon.
    if( ! std::isfinite(expected_mean))
    {
        return 1.0;
    }

    return expected_mean - moves_so_far;
}
