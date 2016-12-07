#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <map>
#include <limits>

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

    std::string trim_outer_whitespace(const std::string& str);
    std::string strip_comments(const std::string& str, char comment);
    std::string strip_block_comment(const std::string& str, char start, char end);
}

namespace Random
{
    double random_normal(double standard_deviation);
    double random_real(double min, double max);
    int random_integer(int min, int max);
    bool coin_flip();
    bool success_probability(double probability);
}

namespace Math
{
    const auto infinity = std::numeric_limits<double>::infinity();

    double average_moves_left(double mean_moves, size_t moves_so_far);
    double poisson_probability(double mean, size_t value);
}

class Configuration_File
{
    public:
        Configuration_File(const std::string& file_name);

        std::string get_text(const std::string& parameter) const;
        double get_number(const std::string& parameter) const;

    private:
        std::map<std::string, std::string> parameters;
};

#endif // UTILITY_H
