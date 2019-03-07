#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>

namespace String
{
    std::vector<std::string> split(std::string s,
                                   std::string delim = "",
                                   size_t count = std::string::npos);

    template<class T>
    bool contains(const std::string& container, const T& target)
    {
        return container.find(target) != std::string::npos;
    }

    bool starts_with(const std::string& s, const std::string& beginning);
    bool ends_with(const std::string& s, const std::string& ending);

    std::string trim_outer_whitespace(const std::string& s);
    std::string remove_extra_whitespace(const std::string& s);

    std::string strip_comments(const std::string& str, const std::string& comment);
    std::string strip_block_comment(const std::string& str, const std::string& start, const std::string& end);

    std::string lowercase(std::string s);
}

#endif // STRING_H
