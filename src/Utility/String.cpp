#include "Utility/String.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <cctype>
#include <iterator>

std::vector<std::string> String::split(const std::string& s, const std::string& delim, const size_t count) noexcept
{
    if(delim.empty())
    {
        auto ss = std::istringstream(s);
        std::vector<std::string> result;
        std::copy(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>(), std::back_inserter(result));
        return result;
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
        end_index = std::min(s.find(delim, start_index), s.size());
        result.push_back(s.substr(start_index, end_index - start_index));
        start_index = end_index + delim.size();
        ++split_count;
    }

    if(start_index <= s.size())
    {
        result.push_back(s.substr(start_index));
    }

    return result;
}

std::string String::trim_outer_whitespace(const std::string& s) noexcept
{
    const auto text_start = std::find_if_not(s.begin(), s.end(), [](auto c) { return std::isspace(c); });
    if(text_start == s.end())
    {
        return {};
    }

    const auto text_end = std::find_if_not(s.rbegin(), s.rend(), [](auto c) { return std::isspace(c); }).base();
    return std::string(text_start, text_end);
}

std::string String::remove_extra_whitespace(const std::string& s) noexcept
{
    const auto words = split(s);
    return join(words.begin(), words.end(), " ");
}

std::string String::strip_comments(const std::string& str, const std::string& comment) noexcept
{
    return trim_outer_whitespace(str.substr(0, str.find(comment)));
}

std::string String::strip_block_comment(const std::string& str, const std::string& start, const std::string& end)
{
    const auto start_comment_index = str.find(start);
    const auto end_comment_index = str.find(end);

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

    try
    {
        const auto first_part = str.substr(0, start_comment_index);
        const auto last_part = str.substr(end_comment_index + end.size());
        return strip_block_comment(trim_outer_whitespace(first_part) + " " + trim_outer_whitespace(last_part), start, end);
    }
    catch(const std::invalid_argument& e)
    {
        throw std::invalid_argument(e.what() + std::string("\nOriginal line: ") + str);
    }
}

std::string String::strip_nested_block_comments(const std::string& str, const std::string& start, const std::string& end)
{
    if(contains(start, end) || contains(end, start))
    {
        throw std::invalid_argument("Delimiters cannot share substrings: " + start + "," + end + ".");
    }

    const auto error_message = "Invalid nesting of delimiters " + start + "," + end + ": " + str;
    std::string result;
    auto depth = 0;
    size_t index = 0;
    while(index < str.size())
    {
        auto start_index = str.find(start, index);
        auto end_index = str.find(end, index);
        if(start_index < end_index)
        {
            if(depth == 0)
            {
                result += str.substr(index, start_index - index);
            }
            ++depth;
            index = start_index + start.size();
        }
        else if(end_index < start_index)
        {
            if(depth == 0)
            {
                throw std::invalid_argument(error_message);
            }
            --depth;
            index = end_index + end.size();
        }
        else // start_index == end_index == std::string::npos
        {
            result += str.substr(index);
            break;
        }
    }

    if(depth != 0)
    {
        throw std::invalid_argument(error_message);
    }

    return result;
}

std::string String::remove_pgn_comments(const std::string& line)
{
    const auto index = line.find_first_of(";({");
    const auto delimiter = index < std::string::npos ? line[index] : '\0';

    switch(delimiter)
    {
        case ';' : return remove_pgn_comments(strip_comments(line, ";"));
        case '(' : return remove_pgn_comments(strip_nested_block_comments(line, "(", ")"));
        case '{' : return remove_pgn_comments(strip_block_comment(line, "{", "}"));
        default  : return remove_extra_whitespace(line);
    }
}

std::string String::extract_delimited_text(const std::string& str, const std::string& start, const std::string& end)
{
    const auto first_delimiter_index = str.find(start);
    if(first_delimiter_index == std::string::npos)
    {
        throw std::invalid_argument("Starting delimiter not found in \"" + str + "\": " + start + " " + end);
    }
    const auto text_start_index = first_delimiter_index + start.size();

    const auto second_delimiter_index = str.find(end, text_start_index);
    if(second_delimiter_index == std::string::npos)
    {
        throw std::invalid_argument("Ending delimiter not found in \"" + str + "\": " + start + " " + end);
    }
    const auto text_length = second_delimiter_index - text_start_index;

    return str.substr(text_start_index, text_length);
}

char String::tolower(const char letter) noexcept
{
    return char(std::tolower(letter));
}

char String::toupper(const char letter) noexcept
{
    return char(std::toupper(letter));
}

std::string String::lowercase(std::string s) noexcept
{
    std::transform(s.begin(), s.end(), s.begin(), String::tolower);
    return s;
}

std::string String::round_to_decimals(const double x, const size_t decimal_places) noexcept
{
    return std::format("{:.{}f}", x, decimal_places);
}

std::string String::add_to_file_name(const std::string& original_file_name, const std::string& addition) noexcept
{
    const auto dot_index = std::min(original_file_name.find_last_of('.'), original_file_name.size());
    return original_file_name.substr(0, dot_index) + addition + original_file_name.substr(dot_index);
}

std::string String::pluralize(int count, const std::string& noun) noexcept
{
    return std::to_string(count) + " " + noun + (count == 1 ? "" : "s");
}
