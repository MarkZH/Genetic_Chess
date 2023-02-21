#include "Utility/Random.h"

#include <random>
#include <string>
#include <limits>
#include <functional>

namespace
{
    struct Seeder
    {
        using result_type = unsigned int;
        template<typename Iterator>
        void generate(const Iterator begin, const Iterator end) const
        {
            std::random_device rd;
            std::generate(begin, end, std::ref(rd));
        }
    };
}

Random::Random_Bits_Generator Random::get_new_seeded_random_bit_source() noexcept
{
    const auto seeder = Seeder{};
    return Random_Bits_Generator(seeder);
}

double Random::random_laplace(const double width) noexcept
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

bool Random::success_probability(const size_t successes, const size_t attempts) noexcept
{
    assert(attempts > 0);
    return random_integer<size_t>(1, attempts) <= successes;
}

std::string Random::random_string(const size_t size) noexcept
{
    std::string s(size, ' ');
    std::ranges::generate(s, []() -> char { return 'a' + char(random_integer(0, 25)); });
    return s;
}
