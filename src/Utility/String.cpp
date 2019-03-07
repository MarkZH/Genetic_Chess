#include "Utility/String.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>

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
    else if(end_index < s.size())
    {
        result.push_back({}); // Original string ended with delimiter
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
        throw std::runtime_error("\"" + str + "\" is missing a comment delimiter: " + std::string{start} + std::string{end});
    }

    if(start_comment_index >= end_comment_index)
    {
        throw std::runtime_error("\"" + str + "\" contains bad comment delimiters: " + std::string{start} + std::string{end});
    }

    auto first_part = str.substr(0, start_comment_index);
    auto last_part = str.substr(end_comment_index + end.size());
    return strip_block_comment(trim_outer_whitespace(first_part) + " " + trim_outer_whitespace(last_part), start, end);
}

std::string String::lowercase(std::string s)
{
    for(auto& c : s){ c = std::tolower(c); }
    return s;
}
