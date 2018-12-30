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
    std::vector<std::string> split(std::string s,
                                   std::string delim = "",
                                   size_t count = std::string::npos);

    template<class T>
    bool contains(const std::string& container, const T& target)
    {
        return container.find(target) != std::string::npos;
    }

    bool starts_with(const std::string& s, const std::string& beginning);
    bool ends_with(const std::string& s, const std::string& ending);

    std::string trim_outer_whitespace(const std::string& s);
    std::string remove_extra_whitespace(const std::string& s);

    std::string strip_comments(const std::string& str, char comment);
    std::string strip_block_comment(const std::string& str, char start, char end);

    std::string lowercase(std::string s);
}

namespace Random
{
    // Random number with Laplace distribution (double-sided exponential) and mean of zero
    double random_laplace(double width);

    // Random number with inclusive range
    double random_real(double min, double max);
    int random_integer(int min, int max);
    uint64_t random_unsigned_int64();

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

        std::string as_text(const std::string& parameter) const;
        double as_number(const std::string& parameter) const;
        bool as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const;

        void print_unused_parameters() const;

    private:
        std::map<std::string, std::string> parameters;
        mutable std::map<std::string, bool> used;

        static std::string standardize_text(const std::string& input);
};

class Scoped_Stopwatch
{
    public:
        explicit Scoped_Stopwatch(const std::string& name);
        Scoped_Stopwatch(const Scoped_Stopwatch& other) = delete;
        Scoped_Stopwatch& operator=(const Scoped_Stopwatch& other) = delete;
        ~Scoped_Stopwatch();

        void stop();
        void add_info(const std::string& info);
        void reject(); // do not record time
        double time_so_far() const;

    private:
        static std::ofstream out_file;
        static std::mutex write_lock;
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
};

#endif // UTILITY_H
