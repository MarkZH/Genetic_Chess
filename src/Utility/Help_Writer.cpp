#include "Utility\Help_Writer.h"

#include <string>
#include <vector>

#include "Utility/String.h"

namespace
{
    const auto line_length = 72;
    const auto indent_size = 4;
}

void Help_Writer::add_title(const std::string& title) noexcept
{
    text += title + '\n' + std::string(title.size(), '=') + "\n\n";
}

void Help_Writer::add_section_title(const std::string& title) noexcept
{
    text += title + ":\n\n";
}

void Help_Writer::add_paragraph(const std::string& paragraph) noexcept
{
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
}

std::string Help_Writer::full_text() const noexcept
{
    return text;
}
