#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <map>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <mutex>

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

    std::string trim_outer_whitespace(const std::string& s);
    std::string consolidate_inner_whitespace(const std::string& s);
    std::string remove_extra_whitespace(const std::string& s);

    std::string strip_comments(const std::string& str, char comment);
    std::string strip_block_comment(const std::string& str, char start, char end);

    std::string lowercase(std::string s);
}

namespace Random
{
    // Random number with normal distribution and mean of zero
    double random_normal(double standard_deviation);

    // Random number with inclusive range
    double random_real(double min, double max);
    int random_integer(int min, int max);

    // Return true with probability 50%
    bool coin_flip();

    // Return true with given probability
    bool success_probability(double probability);

    // Shuffles the order of the list
    template<class List>
    void shuffle(List& list)
    {
        thread_local static std::mt19937_64 generator(std::random_device{}());
        std::shuffle(list.begin(), list.end(), generator);
    }
}

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
}

class Configuration_File
{
    public:
        explicit Configuration_File(const std::string& file_name);

        std::string get_text(const std::string& parameter) const;
        double get_number(const std::string& parameter) const;

    private:
        std::map<std::string, std::string> parameters;
};

class Scoped_Stopwatch
{
    public:
        explicit Scoped_Stopwatch(const std::string& name);
        ~Scoped_Stopwatch();

        void stop();
        void add_info(const std::string& info);
        void reject(); // do not record time

    private:
        static std::ofstream out_file;
        static std::mutex write_lock;
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
};

#endif // UTILITY_H
