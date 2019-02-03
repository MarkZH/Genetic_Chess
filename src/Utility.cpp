#include "Utility.h"

#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>
#include <cmath>

namespace String
{
    namespace
    {
        const auto whitespace = " \t\n";
    }
}

std::vector<std::string> String::split(std::string s, std::string delim, size_t count)
{
    if(delim.empty())
    {
        s = remove_extra_whitespace(s);
        delim = " ";
    }

    std::vector<std::string> result;
    size_t start_index = 0;
    size_t end_index = 0;
    size_t split_count = 0;
    while(end_index < s.size() && split_count < count)
    {
        end_index = s.find(delim, start_index);
        result.push_back(s.substr(start_index, end_index-start_index));
        start_index = std::max(end_index, end_index + delim.size());
        ++split_count;
    }

    if(start_index < s.size())
    {
        result.push_back(s.substr(start_index));
    }

    return result;
}

bool String::starts_with(const std::string& s, const std::string& beginning)
{
    return (beginning.size() <= s.size()) && std::equal(beginning.begin(), beginning.end(), s.begin());
}

bool String::ends_with(const std::string& s, const std::string& ending)
{
    return (ending.size() <= s.size()) && std::equal(ending.rbegin(), ending.rend(), s.rbegin());
}

std::string String::trim_outer_whitespace(const std::string& s)
{
    auto text_start = s.find_first_not_of(whitespace);
    if(text_start == std::string::npos)
    {
        return {};
    }

    auto text_end = s.find_last_not_of(whitespace);
    return s.substr(text_start, text_end - text_start + 1);
}

std::string String::remove_extra_whitespace(const std::string& s)
{
    std::string result;

    for(auto c : trim_outer_whitespace(s))
    {
        if(contains(whitespace, c))
        {
            if(result.back() != ' ')
            {
                result.push_back(' ');
            }
        }
        else
        {
            result.push_back(c);
        }
    }

    return result;
}

std::string String::strip_comments(const std::string& str, char comment)
{
    return trim_outer_whitespace(str.substr(0, str.find(comment)));
}

std::string String::strip_block_comment(const std::string& str, char start, char end)
{
    auto start_comment_index = str.find(start);
    auto end_comment_index = str.find(end);
    
    if(start_comment_index == std::string::npos || end_comment_index == std::string::npos)
    {
        return remove_extra_whitespace(str);
    }

    if(start_comment_index >= end_comment_index)
    {
        throw std::runtime_error("\"" + str + "\" contains bad comment delimiters: " + std::string{start} +std::string{end});
    }

    auto first_part = str.substr(0, start_comment_index);
    auto last_part = str.substr(end_comment_index + 1);
    return strip_block_comment(first_part + " " + last_part, start, end);
}

std::string String::lowercase(std::string s)
{
    for(auto& c : s){ c = std::tolower(c); }
    return s;
}

int Random::random_integer(int min, int max)
{
    thread_local static std::mt19937_64 generator(std::random_device{}());
    using uid = std::uniform_int_distribution<int>;
    thread_local static auto dist = uid{};
    return dist(generator, uid::param_type{min, max});
}

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


// Mean moves left in game given that a number of moves have been made already.
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


Configuration_File::Configuration_File(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    std::string line;
    while(std::getline(ifs, line))
    {
        line = String::strip_comments(line, '#');
        if(line.empty())
        {
            continue;
        }
        if( ! String::contains(line, '='))
        {
            throw std::runtime_error("Configuration file lines must be of form \"Name = Value\"\n" + line);
        }
        auto line_split = String::split(line, "=", 1);
        auto parameter = standardize_text(line_split[0]);
        parameters[parameter] = String::trim_outer_whitespace(line_split[1]);
        used[parameter] = false;
    }
}

std::string Configuration_File::as_text(const std::string& parameter) const
{
    try
    {
        used.at(parameter) = true;
        return parameters.at(standardize_text(parameter));
    }
    catch(const std::out_of_range&)
    {
        for(const auto& key_value : parameters)
        {
            std::cerr << "\"" << key_value.first << "\" --> \"" << key_value.second << "\"" << std::endl;
        }
        throw std::runtime_error("Configuration parameter not found: " + parameter);
    }
}

double Configuration_File::as_number(const std::string& parameter) const
{
    try
    {
        return std::stod(as_text(parameter));
    }
    catch(const std::invalid_argument&)
    {
        throw std::runtime_error("Invalid number for \"" + parameter + "\" : " + as_text(parameter));
    }
}

bool Configuration_File::as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const
{
    auto response = standardize_text(as_text(parameter));
    if(response == standardize_text(affirmative))
    {
        return true;
    }
    else if(response == standardize_text(negative))
    {
        return false;
    }
    else
    {
        throw std::runtime_error("Invalid value for \"" + parameter + "\" : \"" + as_text(parameter) + "\"" +
                                 "\nExpected \"" + affirmative + "\" or \"" + negative + "\".");
    }
}

std::string Configuration_File::standardize_text(const std::string& input)
{
    return String::lowercase(String::remove_extra_whitespace(input));
}

void Configuration_File::print_unused_parameters() const
{
    auto header_printed = false;
    for(const auto& param_value : parameters)
    {
        if( ! used[param_value.first])
        {
            if( ! header_printed)
            {
                std::cout << "Unused configuration parameters:\n";
                header_printed = true;
            }
            std::cout << param_value.first << " --> " << param_value.second << std::endl;
        }
    }
}


std::ofstream Scoped_Stopwatch::out_file;
std::mutex Scoped_Stopwatch::write_lock;

Scoped_Stopwatch::Scoped_Stopwatch(const std::string& name) :
    place_name(name),
    start_time(std::chrono::steady_clock::now()),
    stopped(place_name.empty())
{
}

Scoped_Stopwatch::~Scoped_Stopwatch()
{
    stop();
}

void Scoped_Stopwatch::stop()
{
    auto end_time = std::chrono::steady_clock::now();

    if(stopped)
    {
        return;
    }

    std::lock_guard<std::mutex> write_lock_guard(write_lock);

    if( ! out_file.is_open())
    {
        out_file.open("timings-" + std::to_string(Random::random_unsigned_int64()) + ".txt");
    }

    out_file << place_name << "|"
             << std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count()
             << '\n';

    stopped = true;
}

void Scoped_Stopwatch::add_info(const std::string& info)
{
    place_name += info;
}

void Scoped_Stopwatch::reject()
{
    stopped = true;
}

double Scoped_Stopwatch::time_so_far() const
{
    auto end_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count();
}
