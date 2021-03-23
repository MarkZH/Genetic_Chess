#include "Utility/Random.h"

#include <random>
#include <string>
#include <limits>

namespace
{
    struct Seeder
    {
        using result_type = unsigned int;
        template<typename Iterator>
        void generate(Iterator begin, Iterator end)
        {
            std::random_device rd;
            std::generate(begin, end, [&rd]() { return rd(); });
        }
    };
}

Random::Random_Bits_Generator Random::get_new_seeded_random_bit_source() noexcept
{
    auto seeder = Seeder{};
    return Random_Bits_Generator(seeder);
}

double Random::random_laplace(double width) noexcept
{
    thread_local static auto generator = get_new_seeded_random_bit_source();
    using ed = std::exponential_distribution<double>;
    thread_local static auto dist = ed{};
    return (coin_flip() ? 1 : -1)*dist(generator, ed::param_type{1.0/width});
}

uint64_t Random::random_unsigned_int64() noexcept
{
    return random_integer(std::numeric_limits<uint64_t>::min(),
                          std::numeric_limits<uint64_t>::max());
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
