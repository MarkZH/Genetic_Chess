#include "Utility\Help_Writer.h"

#include <string>
#include <vector>
#include <iostream>

#include "Utility/String.h"

namespace
{
    const auto line_length = 72;
    const auto indent_size = 4;
}

void Help_Writer::add_title(const std::string& title) noexcept
{
    add_extra_space_after_option();
    text += title + '\n' + std::string(title.size(), '=') + "\n\n";
}

void Help_Writer::add_section_title(const std::string& title) noexcept
{
    add_extra_space_after_option();
    const auto small_indent = std::string(indent_size/2, ' ');
    text += small_indent + title + "\n" + small_indent + std::string(title.size(), '-') + "\n";
}

void Help_Writer::add_paragraph(const std::string& paragraph) noexcept
{
    add_extra_space_after_option();
    text += String::word_wrap(line_length, 0, paragraph) + "\n\n";
}

void Help_Writer::add_option(const std::string& name,
                             const std::vector<std::string>& required_parameters,
                             const std::vector<std::string>& optional_parameters,
                             const std::string& description) noexcept
{
    text += std::string(indent_size, ' ') + name;
    
    for(const auto& parameter : required_parameters)
    {
        text += " [" + parameter + "]";
    }

    for(const auto& parameter : optional_parameters)
    {
        text += " <" + parameter + ">";
    }

    text += "\n";

    if( ! description.empty())
    {
        text += String::word_wrap(line_length, 2*indent_size, description) + "\n\n";
    }
    need_extra_space = description.empty();
}

void Help_Writer::add_option(const std::string& name, const std::vector<std::string>& required_parameters, const std::string& description) noexcept
{
    add_option(name, required_parameters, {}, description);
}

void Help_Writer::add_option(const std::string& name, const std::string& description) noexcept
{
    add_option(name, {}, {}, description);
}

void Help_Writer::add_extra_space_after_option() noexcept
{
    if(need_extra_space)
    {
        text += '\n';
        need_extra_space = false;
    }
}

std::ostream& operator<<(std::ostream& os, const Help_Writer& help) noexcept
{
    return (os << help.text);
}
