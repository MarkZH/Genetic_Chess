#include "Utility/String.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <ranges>

std::vector<std::string> String::split(const std::string& s, const std::string& delim, const size_t count) noexcept
{
    const auto to_vector = std::ranges::to<std::vector<std::string>>();

    const auto initial_take = std::min(count, s.size());
    auto split_view = std::views::split(s, delim);
    auto result = split_view | std::views::take(initial_take) | to_vector;
    const auto remainder = split_view | std::views::drop(initial_take) | to_vector;
    if( ! remainder.empty())
    {
        result.push_back(join(remainder, delim));
    }
    return result;
}

std::vector<std::string> String::split(const std::string& s) noexcept
{
    return s | std::views::transform([](auto c) { return isspace(c) ? ' ' : c; })
             | std::views::split(' ')
             | std::views::filter([](const auto& ss) { return ! ss.empty(); })
             | std::ranges::to<std::vector<std::string>>();
}

std::string String::trim_outer_whitespace(const std::string& s) noexcept
{
    const auto text_start = std::ranges::find_if_not(s, String::isspace);
    if(text_start == s.end())
    {
        return {};
    }

    const auto text_end = std::ranges::find_if_not(std::ranges::reverse_view(s), String::isspace).base();
    return std::string(text_start, text_end);
}

std::string String::remove_extra_whitespace(const std::string& s) noexcept
{
    return join(split(s), " ");
}

std::string String::strip_comments(const std::string& str, const std::string& comment) noexcept
{
    return trim_outer_whitespace(str.substr(0, str.find(comment)));
}

std::string String::extract_delimited_text(const std::string& str, const char start, const char end)
{
    const auto first_delimiter_index = str.find(start);
    if(first_delimiter_index == std::string::npos)
    {
        throw std::invalid_argument(std::format("Starting delimiter not found in \"{}\": {} {}", str, start, end));
    }
    const auto text_start_index = first_delimiter_index + 1;

    const auto second_delimiter_index = str.find(end, text_start_index);
    if(second_delimiter_index == std::string::npos)
    {
        throw std::invalid_argument(std::format("Ending delimiter not found in \"{}\": {} {}", str, start, end));
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
    std::ranges::transform(s, s.begin(), String::tolower);
    return s;
}

bool String::isspace(char c) noexcept
{
    return std::isspace(c);
}

std::string String::word_wrap(const std::string& text, const size_t line_length, const size_t indent) noexcept
{
    const std::string space = " ";
    const auto indent_space = std::string(indent, ' ');

    std::vector<std::string> lines;
    for(const auto& word : split(text))
    {
        if(lines.empty() || lines.back().size() + space.size() + word.size() > line_length)
        {
            lines.push_back(indent_space + word);
        }
        else
        {
            lines.back() += space + word;
        }
    }

    auto wrapped = join(lines, "\n");
    std::ranges::replace(wrapped, '~', ' ');
    return wrapped;
}
