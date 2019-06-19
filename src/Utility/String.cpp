#include "Utility/String.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <sstream>
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

    if(s.empty())
    {
        return {};
    }

    std::vector<std::string> result;
    size_t start_index = 0;
    size_t end_index = 0;
    size_t split_count = 0;
    while(end_index < s.size() && split_count < count)
    {
        end_index = s.find(delim, start_index);
        result.push_back(s.substr(start_index, end_index - start_index));
        start_index = std::min(end_index, s.size()) + delim.size();
        ++split_count;
    }

    if(start_index <= s.size())
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
    std::string s2 = trim_outer_whitespace(s);
    std::replace_if(s2.begin(), s2.end(), [](auto c) { return String::contains(whitespace, c); }, ' ');
    std::string result;
    std::copy_if(s2.begin(), s2.end(), std::back_inserter(result),
                 [&result](auto c)
                 {
                     return  c != ' ' || result.back() != ' ';

                 });

    return result;
}

std::string String::strip_comments(const std::string& str, const std::string& comment)
{
    return trim_outer_whitespace(str.substr(0, str.find(comment)));
}

std::string String::strip_block_comment(const std::string& str, const std::string& start, const std::string& end)
{
    auto start_comment_index = str.find(start);
    auto end_comment_index = str.find(end);

    if(start_comment_index == std::string::npos && end_comment_index == std::string::npos)
    {
        return trim_outer_whitespace(str);
    }

    if(start_comment_index == std::string::npos || end_comment_index == std::string::npos)
    {
        throw std::invalid_argument("\"" + str + "\" is missing a comment delimiter: " + start + end);
    }

    if(start_comment_index >= end_comment_index)
    {
        throw std::invalid_argument("\"" + str + "\" contains bad comment delimiters: " + start + end);
    }

    auto first_part = str.substr(0, start_comment_index);
    auto last_part = str.substr(end_comment_index + end.size());
    try
    {
        return strip_block_comment(trim_outer_whitespace(first_part) + " " + trim_outer_whitespace(last_part), start, end);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::invalid_argument(e.what() + std::string("\nOriginal line: ") + str);
    }
}

std::string String::lowercase(std::string s)
{
    for(auto& c : s){ c = std::tolower(c); }
    return s;
}

std::string String::format_integer(int n, const std::string& separator)
{
    if(n < 0)
    {
        return '-' + format_integer(-n, separator);
    }
    else
    {
        auto s = std::to_string(n);
        auto group_size = 3;
        auto index = s.size() % group_size;
        index = (index == 0 ? group_size : index);
        auto result = s.substr(0, index);

        for( ; index < s.size(); index += group_size)
        {
            result += separator;
            result += s.substr(index, group_size);
        }

        return result;
    }
}

std::string String::round_to_precision(double x, double precision)
{
    auto result = std::to_string(std::round(x/precision)*precision);
    auto last_significant_index = result.find_last_not_of('0');
    if(last_significant_index == std::string::npos)
    {
        return result;
    }
    else
    {
        if(result[last_significant_index] == '.')
        {
            if(precision < 1.0)
            {
                // Include ".0"
                return result.substr(0, last_significant_index + 2);
            }
            else
            {
                // Precision is greater than one, so delete fractional part
                return result.substr(0, last_significant_index);
            }
        }
        else
        {
            // Include first non-significant digit
            return result.substr(0, last_significant_index + 1);
        }
    }
}

size_t String::string_to_size_t(const std::string& s)
{
    size_t characters_used;

    auto result =
    #ifdef __linux__
        std::stoul(s, &characters_used);
    #elif defined(_WIN64)
        std::stoull(s, &characters_used);
    #else
        std::stoi(s, &characters_used);
    #endif

    if( ! String::trim_outer_whitespace(s.substr(characters_used)).empty())
    {
        throw std::invalid_argument("Non-numeric characters in string: " + s);
    }

    return result;
}

std::string String::date_and_time_format(const std::chrono::system_clock::time_point& point_in_time,
                                         const std::string& format)
{
    auto time_c = std::chrono::system_clock::to_time_t(point_in_time);
    std::tm time_out;
#ifdef _WIN32
    localtime_s(&time_out, &time_c);
#elif defined(__linux__)
    localtime_r(&time_c, &time_out);
#endif
    auto ss = std::ostringstream{};
    ss << std::put_time(&time_out, format.c_str());
    return ss.str();
}
