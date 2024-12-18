#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>
#include <chrono>
#include <type_traits>
#include <algorithm>
#include <concepts>
#include <locale>
#include <charconv>
#include <stdexcept>
#include <format>
#include <ranges>
#include <iostream>

//! \brief A collection of useful functions for dealing with text strings.
namespace String
{
    //! \brief Splits a string into a list of strings based upon a delimiter.
    //!
    //! \param s The string to be split.
    //! \param delim The delimiter that marks where the string should be split.
    //! \param count The maximum number of splits. The final number of strings will be
    //!        count + 1 at most. If unspecified, as many splits as possible will be done.
    //! \returns A list of strings that were separated by the delimiter.
    std::vector<std::string> split(const std::string& s,
                                   const std::string& delim,
                                   size_t count = std::string::npos) noexcept;

    //! \brief Splits a string into a list of strings by cutting at whitespace.
    //!
    //! \param s The string to be split.
    //! \returns A list of strings that were separated by whitespace. None of the strings
    //!          in the resulting list will have any whitespace.
    std::vector<std::string> split(const std::string& s) noexcept;

    //! \brief Join a sequence of strings into a single string with joiner strings in between.
    //!
    //! \tparam Container A container with ordered contents.
    //! \param container A container of items convertible to std::string.
    //! \param joiner A string that will be placed between every string in the sequence.
    template<typename Container>
    std::string join(const Container& container, const std::string& joiner) noexcept
    {
        return container | std::ranges::views::join_with(joiner) | std::ranges::to<std::string>();
    }

    //! \brief Remove leading and trailing whitespace from a string.
    //!
    //! \param s The input string.
    //! \returns The same string with no leading or trailing whitespace.
    std::string trim_outer_whitespace(const std::string& s) noexcept;

    //! \brief Remove leading, trailing, and multiple whitespaces from a string.
    //!
    //! \param s The input string.
    //! \returns The same string with no leading or trailing whitespace, and with all
    //!          runs of internal whitespace replaced with a single space.
    std::string remove_extra_whitespace(const std::string& s) noexcept;

    //! \brief Remove all text after a comment indicator.
    //!
    //! \param str The input string.
    //! \param comment The marker indicating the start of a comment to be removed.
    //! \returns The same string with everything after the comment marker removed.
    std::string strip_comments(const std::string& str, const std::string& comment) noexcept;

    //! \brief Extract text between delimiters
    //!
    //! \param str The input string
    //! \param start The marker indicating the beginning of the text block.
    //! \param end The marker indicating the end of the text block.
    //! Note: The returned text will not include the delimiters. This function does not consider
    //!       delimiter nesting, meaning it will use the first instance of start and the first
    //!       instance of end that follows start.
    //!
    //! \code{cpp}
    //! extract_delimited_text("(a(b))", "(", ")") == "a(b"
    //! \endcode
    std::string extract_delimited_text(const std::string& str, char start, char end);

    //! \brief Typesafe single-character lowercase function.
    //!
    //! \param letter The letter to change to lowercase.
    char tolower(char letter) noexcept;

    //! \brief Typesafe single-character uppercase function.
    //!
    //! \param letter The letter to change to uppercase.
    char toupper(char letter) noexcept;

    //! \brief Create a lowercase version of a string.
    //!
    //! \param s The input string.
    //! \returns The same string with all letters replaced by their lowercase versions.
    std::string lowercase(std::string s) noexcept;

    //! \brief Check if a character is whitespace.
    //!
    //! This is useful with std:: algorithms since std::isspace takes an int as a parameter.
    bool isspace(char c) noexcept;

    //! \brief Returns a text version of an integer with thousands separators (US-style)
    //!
    //! \param n The integer.
    //! \returns A text string with thousands separators.
    template<typename Number> requires std::is_arithmetic_v<Number>
    std::string format_number(Number n) noexcept
    {
        struct thousands_separator : std::numpunct<char>
        {
            char do_thousands_sep() const override { return ','; }
            std::string do_grouping() const override { return "\3"; }
        };

        return std::format(std::locale(std::cout.getloc(), new thousands_separator), "{:L}", n);
    }

    //! \brief Convert a std::string to a numeric type.
    //!
    //! \param s The input string containing a number.
    //! \tparam Number The numeric type the string should be converted to.
    //! \returns A number of type Number.
    //! \exception std::invalid_argument if no conversion could be made or if there are extra characters
    //!         that cannot be converted to a number.
    template<typename Number> requires std::is_arithmetic_v<Number>
    Number to_number(const std::string& s)
    {
        const auto trimmed = trim_outer_whitespace(s);
        const auto string_end = trimmed.data() + trimmed.size();
        Number result;
        const auto [end, error] = std::from_chars(trimmed.data(), string_end, result);
        if(end == string_end && error == std::errc{})
        {
            return result;
        }
        else
        {
            throw std::invalid_argument(std::format("Non-numeric data in argument: {}", s));
        }
    }

    //! \brief Convert a string to a type derived from std::chrono::duration.
    //!
    //! \param s The string to be converted.
    //! \tparam Duration The std::chrono::duration type.
    //! \returns A time duration of type Duration.
    //! \exception std::invalid_argument If no conversion could be made or if there are any
    //!         non-numeric characters in the input string.
    template<typename Duration>
    Duration to_duration(const std::string& s)
    {
        return Duration{to_number<typename Duration::rep>(s)};
    }

    //! Wrapper for std::vformat that's easier to use.
    //! 
    //! \tparam Format_Args The types of data that will populate the format string.
    //! \param format_template A string representing a format template in the same way as std::format() and std::vformat().
    //! \param args The data to populate the format string.
    //! \returns A std::string formatted with the provided args data.
    template<typename ...Format_Args>
    std::string sformat(const std::string& format_template, const Format_Args&... args)
    {
        return std::vformat(format_template, std::make_format_args(args...));
    }

    //! \brief Create a text string of the given time point in the given format
    //!
    //! \param point_in_time The time point to convert.
    //! \param format The format of the date/time data (see docs for std::put_time).
    //! \tparam Precision A std::chrono::duration type that indicates how precise the time should be printed.
    //! \tparam Time_Point The std::chrono::time_point type.
    //! \returns A formatted text string of the date and/or time.
    template<typename Precision = std::chrono::seconds, typename Time_Point>
    std::string date_and_time_format(const Time_Point& point_in_time,
                                     const std::string& format) noexcept
    {
        const auto point_with_tz = std::chrono::zoned_time(std::chrono::current_zone(),
                                                           std::chrono::time_point_cast<Precision>(point_in_time));
        return sformat(std::format("{{:{}}}", format), point_with_tz);
    }

    //! \brief Create a strings with added line breaks so no line is longer than a limit.
    //!
    //! \param line_length The maximum length of a line in the wrapped text (included the indent).
    //! \param indent The number of spaces to indent each line of text.
    //! \param text The raw text.
    //!
    //! All whitespace will be condensed to single spaces before wrapping. The character ~ will be
    //! converted to a non-breaking space.
    std::string word_wrap(const std::string& text, size_t line_length, size_t indent = 0) noexcept;
}

#endif // STRING_H
