#include "Utility/String.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <cctype>
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
    std::ranges::transform(s, s.begin(), String::tolower);
    return s;
}

bool String::isdigit(const char c) noexcept
{
    return std::isdigit(c);
}

bool String::isspace(char c) noexcept
{
    return std::isspace(c);
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

std::string String::pluralize(const size_t count, const std::string& noun) noexcept
{
    return std::to_string(count) + " " + noun + (count == 1 ? "" : "s");
}

std::string String::word_wrap(const std::string& text, const size_t line_length, const size_t indent) noexcept
{
    const auto text_length = line_length - indent;

    std::vector<std::string> lines{""};
    for(const auto& word : split(text))
    {
        std::string space = lines.back().empty() ? "" : " ";
        if(lines.back().size() + space.size() + word.size() > text_length)
        {
            lines.push_back("");
            space = "";
        }
        lines.back() += space + word;
    }

    const auto indent_space = std::string(indent, ' ');
    auto wrapped = indent_space + join(lines, "\n" + indent_space);
    std::ranges::replace(wrapped, '~', ' ');
    return wrapped;
}
