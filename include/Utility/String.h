#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>
#include <chrono>

//! A collection of useful functions for dealing with text strings.
namespace String
{
    //! Splits a string into a list of strings based upon a delimiter.
    //
    //! \param s The string to be split.
    //! \param delim The delimiter that marks where the string should be split. If empty,
    //!        any number of consecutive whitespaces will be the delimiter.
    //! \param count The maximum number of splits. The final number of strings will be
    //!        count + 1 at most.
    //! \returns A list of strings that were separated by the delimiter.
    std::vector<std::string> split(std::string s,
                                   std::string delim = "",
                                   size_t count = std::string::npos) noexcept;

    //! Determine whether a string exists inside another string.
    //
    //! \param container The string to search.
    //! \param target The character or string to search for.
    //! \returns True if the target is inside the container.
    template<class T>
    bool contains(const std::string& container, const T& target) noexcept
    {
        return container.find(target) != std::string::npos;
    }

    //! Determine whether a strings has another string as a prefix.
    //
    //! \param s The string to check.
    //! \param beginning The prefix to find.
    //! \returns True if the string starts with the beginning string.
    bool starts_with(const std::string& s, const std::string& beginning) noexcept;

    //! Determine whether a strings has another string as a suffix.
    //
    //! \param s The string to check.
    //! \param ending The suffix to find.
    //! \returns True if the string ends with the ending string.
    bool ends_with(const std::string& s, const std::string& ending) noexcept;

    //! Remove leading and trailing whitespace from a string.
    //
    //! \param s The input string.
    //! \returns The same string with no leading or trailing whitespace.
    std::string trim_outer_whitespace(const std::string& s) noexcept;

    //! Remove leading, trailing, and multiple whitespaces from a string.
    //
    //! \param s The input string.
    //! \returns The same string with no leading or trailing whitespace, and with all
    //!          runs of internal whitespace replaced with a single space.
    std::string remove_extra_whitespace(const std::string& s) noexcept;

    //! Remove all text after a comment indicator.
    //
    //! \param str The input string.
    //! \param comment The marker indicating the start of a comment to be removed.
    //! \returns The same string with everything after the comment marker removed.
    std::string strip_comments(const std::string& str, const std::string& comment) noexcept;

    //! Remove all text between comment indicators.
    //
    //! \param str The input string.
    //! \param start The marker indicating the start of a comment to be removed.
    //! \param end The marker indicating the end of a comment to be removed.
    //! \returns The same string with the markers and everything between them removed.
    std::string strip_block_comment(const std::string& str, const std::string& start, const std::string& end);

    //! Extract text between delimiters
    //
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
    std::string extract_delimited_text(const std::string& str, const std::string& start, const std::string& end);

    //! Create a lowercase version of a string.
    //
    //! \param s The input string.
    //! \returns The same string with all letters replaced by their lowercase versions.
    std::string lowercase(std::string s) noexcept;

    //! Returns a text version of an integer with thousands separators
    //
    //! \param n The integer.
    //! \param separator The separator between groups of thousands.
    //! \returns A text string with thousands separators.
    template<typename Integer>
    constexpr std::enable_if_t<std::is_integral_v<Integer>, std::string> format_integer(Integer n, const std::string& separator) noexcept
    {
        if constexpr (std::is_signed_v<Integer>)
        {
            if(n < 0)
            {
                return '-' + format_integer(-n, separator);
            }
        }

        auto s = std::to_string(n);
        auto group_size = 3;
        auto index = s.size() % group_size;
        index = (index == 0 ? group_size : index);
        auto result = s.substr(0, index);

        for(; index < s.size(); index += group_size)
        {
            result += separator;
            result += s.substr(index, group_size);
        }

        return result;
    }

    //! Round a number to the specified precision
    //
    //! \param x The number to be rounded.
    //! \param precision The precision of the rounding (0.01 for 2 decimal places of precision).
    //! \returns A string representation of the rounded number.
    std::string round_to_precision(double x, double precision) noexcept;

    //! Convert a std::string to a size_t for multiple platforms.
    //
    //! \param s The input string containing a number.
    //! \returns An unsigned integer of the same type as size_t.
    //! \throws std::invalid_argument if no conversion could be made.
    //! \throws std::out_of_range if the number in the string cannot fit in a size_t.
    size_t string_to_size_t(const std::string& s);

    //! Create a text string of the given time point in the given format
    //
    //! \param point_in_time The time point to convert.
    //! \param format The format of the date/time data (see docs for std::put_time).
    //! \returns A formatted text string of the date and/or time.
    std::string date_and_time_format(const std::chrono::system_clock::time_point& point_in_time,
                                     const std::string& format) noexcept;

    //! Inserts extra text before the file name extension.
    //
    //! \param original_file_name The file name to be editted.
    //! \param addition A string to insert just before the dot before the file name extension.
    //! \returns (original file name without extention) + (addition) + (extension)
    std::string add_to_file_name(const std::string& original_file_name,
                                 const std::string& addition) noexcept;
}

#endif // STRING_H
