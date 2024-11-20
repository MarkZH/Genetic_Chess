#ifndef MAIN_TOOL_H
#define MAIN_TOOL_H

#include <string>
#include <vector>
#include <tuple>
#include <stdexcept>

#include "String.h"

namespace Main_Tools
{
    using command_line_options = std::vector<std::tuple<std::string, std::vector<std::string>>>;

    //! \brief Print the command-line options for this program.
    void print_help();

    //! \brief Throws std::invalid_argument if assertion fails
    //!
    //! \param condition A condition that must be true to continue.
    //! \param failure_message A message to display if the assertion fails.
    template<typename ...Format_Args>
    void argument_assert(bool condition, const std::string& failure_template, const Format_Args&... args)
    {
        if( ! condition)
        {
            throw std::invalid_argument(String::format_message(failure_template, args...));
        }
    }

    //! \brief Standardizes command line options into a vector of strings.
    command_line_options parse_options(int argc, char* argv[]);
}

#endif // MAIN_TOOL_H
