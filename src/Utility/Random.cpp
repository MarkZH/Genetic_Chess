#include "Utility/Random.h"

#include <random>
#include <string>

double Random::random_laplace(double width) noexcept
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    using ed = std::exponential_distribution<double>;
    thread_local static auto dist = ed{};
    return (coin_flip() ? 1 : -1)*dist(generator, ed::param_type{1.0/width});
}

uint64_t Random::random_unsigned_int64() noexcept
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    thread_local static std::uniform_int_distribution<uint64_t> dist;
    return dist(generator);
}

bool Random::coin_flip() noexcept
{
    return success_probability(1, 2);
}

bool Random::success_probability(size_t successes, size_t attempts) noexcept
{
    assert(attempts > 0);
    return random_integer(size_t{1}, attempts) <= successes;
}

std::string Random::random_string(size_t size) noexcept
{
    std::string s;
    while(s.size() < size)
    {
        s.push_back('a' + random_integer(0, 25));
    }
    return s;
}
