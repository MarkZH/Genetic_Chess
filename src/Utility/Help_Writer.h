#ifndef HELP_WRITER_H

#include <string>
#include <vector>
#include <iosfwd>

//! \brief A tool for printing command line documentation.
class Help_Writer
{
    public:
        //! \brief Create a title for the documentation.
        void add_title(const std::string& title) noexcept;

        //! \brief Create a section header.
        void add_section_title(const std::string& title) noexcept;

        //! \brief Add a paragraph with word wrapping.
        void add_paragraph(const std::string& paragraph) noexcept;

        //! \brief Add documentation for a command line option.
        //!
        //! \param name The name of the option as written on the command line (e.g., "-help")
        //! \param required_parameters A list of parameters that are required for the option (will be printed with [square brackets]).
        //! \param optional_parameters A list of parameters that are optional for the option (will be printed with \<angle brackets\>).
        //! \param description A description of the option. The text will be indented and word wrapped.
        void add_option(const std::string& name,
                        const std::vector<std::string>& required_parameters = {},
                        const std::vector<std::string>& optional_parameters = {},
                        const std::string& description = {}) noexcept;

        //! \brief Add documentation for a command line option with only required parameters.
        //!
        //! \param name The name of the option as written on the command line (e.g., "-help")
        //! \param required_parameters A list of parameters that are required for the option (will be printed with [square brackets]).
        //! \param description A description of the option. The text will be indented and word wrapped.
        void add_option(const std::string& name,
                        const std::vector<std::string>& required_parameters,
                        const std::string& description) noexcept;

        //! \brief Add documentation for a command line option with no parameters.
        //!
        //! \param name The name of the option as written on the command line (e.g., "-help")
        //! \param description A description of the option. The text will be indented and word wrapped.
        void add_option(const std::string& name,
                        const std::string& description) noexcept;

        std::string output() const noexcept;

    private:
        std::string text;
        bool need_extra_space = false;

        void add_extra_space_after_option() noexcept;
};

#endif // HELP_WRITER_H
