#include "Utility.h"

#include <vector>
#include <random>
#include <chrono>

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

int Random::random_integer(int min, int max)
{
    static std::default_random_engine
        generator(std::chrono::system_clock::now().time_since_epoch().count());
    return std::uniform_int_distribution<int>(min, max)(generator);
}

double Random::random_normal(double standard_deviation)
{
    static std::default_random_engine
        generator(std::chrono::system_clock::now().time_since_epoch().count());
    return std::normal_distribution<double>(0, standard_deviation)(generator);
}

bool Random::coin_flip()
{
    return random_integer(0, 1) == 0;
}
