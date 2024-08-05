#ifndef TOOLING_H

#include <string>
#include <vector>
#include <tuple>

namespace Main_Tools
{
    //! \brief Print the command-line options for this program.
    void print_help();

    //! \brief Throws std::invalid_argument if assertion fails
    //!
    //! \param condition A condition that must be true to continue.
    //! \param failure_message A message to display if the assertion fails.
    void argument_assert(bool condition, const std::string& failure_message);

    //! \brief Standardizes command line options into a vector of strings.
    std::vector<std::tuple<std::string, std::vector<std::string>>> parse_options(int argc, char* argv[]);
}

#endif // TOOLING_H