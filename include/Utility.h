#ifndef UTILITY_H
#define UTILITY_H

#include <sstream>
#include <vector>

namespace String
{
    std::vector<std::string> split(const std::string& s,
                                   const std::string& delim = "",
                                   size_t count = std::string::npos);

    template<class T>
    bool contains(const std::string& container, const T& target)
    {
        return container.find(target) != std::string::npos;
    }

    bool starts_with(const std::string& s, const std::string& beginning);
    bool starts_with(const std::string& s, char beginning);
}

namespace Random
{
    double random_normal(double standard_deviation);
    int random_integer(int min, int max);
    bool coin_flip();
}

#endif // UTILITY_H
