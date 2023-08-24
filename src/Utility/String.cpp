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
#include <cctype>
#include <iterator>

std::vector<std::string> String::split(const std::string& s, const std::string& delim, const size_t count) noexcept
{
    std::vector<std::string> result;
    auto word_start = s.begin();
    for(size_t split_count = 0; word_start != s.end() && split_count < count; ++split_count)
    {
        const auto word_end = std::search(word_start, s.end(), delim.begin(), delim.end());
        result.emplace_back(word_start, word_end);
        word_start = word_end == s.end() ? s.end() : std::next(word_end, delim.size());
    }

    if(word_start != s.end())
    {
        result.emplace_back(word_start, s.end());
    }
    else if(String::ends_with(s, delim))
    {
        result.emplace_back();
    }

    return result;
}

std::vector<std::string> String::split(const std::string& s) noexcept
{
    std::vector<std::string> result;
    auto word_start = std::find_if_not(s.begin(), s.end(), isspace);
    while(word_start != s.end())
    {
        const auto word_end = std::find_if(word_start, s.end(), isspace);
        result.emplace_back(word_start, word_end);
        word_start = std::find_if_not(word_end, s.end(), isspace);
    }
    return result;
}

bool String::starts_with(const std::string& s, const std::string& beginning) noexcept
{
    return std::mismatch(beginning.begin(), beginning.end(), s.begin(), s.end()).first == beginning.end();
}

bool String::ends_with(const std::string& s, const std::string& beginning) noexcept
{
    return std::mismatch(beginning.rbegin(), beginning.rend(), s.rbegin(), s.rend()).first == beginning.rend();
}

std::string String::trim_outer_whitespace(const std::string& s) noexcept
{
    const auto text_start = std::find_if_not(s.begin(), s.end(), String::isspace);
    if(text_start == s.end())
    {
        return {};
    }

    const auto text_end = std::find_if_not(s.rbegin(), s.rend(), String::isspace).base();
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
    std::transform(s.begin(), s.end(), s.begin(), String::tolower);
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
    auto result = std::ostringstream();
    result << std::fixed << std::setprecision(int(decimal_places)) << x;
    return result.str();
}

std::string String::date_and_time_format(const std::chrono::system_clock::time_point& point_in_time,
                                         const std::string& format) noexcept
{
    const auto time_c = std::chrono::system_clock::to_time_t(point_in_time);
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
    std::replace(wrapped.begin(), wrapped.end(), '~', ' ');
    return wrapped;
}
