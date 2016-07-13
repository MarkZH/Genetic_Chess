#include "Utility.h"

#include <vector>
#include <random>
#include <chrono>
#include <fstream>

std::vector<std::string> String::split(const std::string& s, const std::string& delim, size_t count)
{
    std::vector<std::string> result;
    size_t start_index = 0;
    size_t end_index = 0;
    size_t split_count = 0;
    while(end_index < s.size() && split_count < count)
    {
        end_index = (delim.empty() ?
                     s.find_first_of(" \t\n", start_index) :
                     s.find(delim, start_index));
        result.push_back(s.substr(start_index, end_index-start_index));
        if(end_index == std::string::npos)
        {
            start_index = std::string::npos;
        }
        else
        {
            start_index = end_index + (delim.empty() ? 1 : delim.size());
        }
        ++split_count;
    }

    if(start_index < s.size())
    {
        result.push_back(s.substr(start_index));
    }

    if(delim.empty())
    {
        auto it = result.begin();
        while(it != result.end())
        {
            if((*it).empty())
            {
                it = result.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return result;
}

bool String::starts_with(const std::string& s, const std::string& beginning)
{
    if(beginning.size() > s.size())
    {
        return false;
    }

    for(size_t i = 0; i < beginning.size(); ++i)
    {
        if(beginning[i] != s[i])
        {
            return false;
        }
    }

    return true;
}

bool String::starts_with(const std::string& s, char beginning)
{
    return s[0] == beginning;
}

std::string String::trim_outer_whitespace(const std::string& str)
{
    size_t start = 0;
    while(start < str.size() && isspace(str[start]))
    {
        ++start;
    }

    size_t end = std::max(str.size() - 1, start);
    while(end > start && isspace(str[end]))
    {
        --end;
    }

    return str.substr(start, end - start + 1);
}

std::string String::strip_comments(const std::string& str, char comment)
{
    return trim_outer_whitespace(str.substr(0, str.find(comment)));
}

int Random::random_integer(int min, int max)
{
    static std::mt19937_64
        generator(std::chrono::system_clock::now().time_since_epoch().count());
    return std::uniform_int_distribution<int>(min, max)(generator);
}

double Random::random_normal(double standard_deviation)
{
    static std::mt19937_64
        generator(std::chrono::system_clock::now().time_since_epoch().count());
    return std::normal_distribution<double>(0, standard_deviation)(generator);
}

double Random::random_real(double min, double max)
{
    static std::mt19937_64
        generator(std::chrono::system_clock::now().time_since_epoch().count());
    return std::uniform_real_distribution<double>(min, max)(generator);
}

bool Random::coin_flip()
{
    return random_integer(0, 1) == 0;
}

bool Random::success_probability(double probability)
{
    return random_real(0, 1) < probability;
}


Configuration_File::Configuration_File(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    std::string line;
    while(getline(ifs, line))
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
        parameters[String::trim_outer_whitespace(line_split[0])] = line_split[1];
    }
}

std::string Configuration_File::get_text(const std::string& parameter) const
{
    try
    {
        return String::trim_outer_whitespace(parameters.at(parameter));
    }
    catch(const std::out_of_range&)
    {
        return "";
    }
}

double Configuration_File::get_number(const std::string& parameter) const
{
    try
    {
        return std::stod(get_text(parameter));
    }
    catch(const std::invalid_argument&)
    {
        return 0.0;
    }
}
