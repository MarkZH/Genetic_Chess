#include "Utility/Random.h"

#include <random>

double Random::random_laplace(double width)
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    using ed = std::exponential_distribution<double>;
    thread_local static auto dist = ed{};
    return (coin_flip() ? 1 : -1)*dist(generator, ed::param_type{1.0/width});
}

double Random::random_real(double min, double max)
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    using urd = std::uniform_real_distribution<double>;
    thread_local static auto dist = urd{};
    return dist(generator, urd::param_type{min, max});
}

uint64_t Random::random_unsigned_int64()
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    thread_local static std::uniform_int_distribution<uint64_t> dist;
    return dist(generator);
}

bool Random::coin_flip()
{
    return random_integer(0, 1) == 1;
}

bool Random::success_probability(double probability)
{
    return random_real(0.0, 1.0) < probability;
}
