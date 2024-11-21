#include "Utility/Configuration.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <format>

#include "Utility/String.h"

namespace
{
    void print_key_value_pair(std::ostream& os, const std::string& key, const std::string& value) noexcept
    {
        os << std::quoted(key) << " --> " << std::quoted(value) << "\n";
    }
}

Configuration::Configuration(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error(std::format("Could not open configuration file: {}", file_name));
    }

    for(std::string line; std::getline(ifs, line);)
    {
        line = String::strip_comments(line, "#");
        if(line.empty())
        {
            continue;
        }

        if( ! String::contains(line, '='))
        {
            throw std::runtime_error(std::format("Configuration file lines must be of form \"Name = Value\"\n{}", line));
        }

        const auto line_split = String::split(line, "=", 1);
        const auto parameter = standardize_text(line_split[0]);
        const auto value = String::trim_outer_whitespace(line_split[1]);

        if(parameter.empty() || value.empty())
        {
            throw std::runtime_error(std::format("Configuration parameter cannot be empty.\n{}", line));
        }

        if(parameters.count(parameter) > 0)
        {
            throw std::runtime_error(std::format("Configuration parameter used more than once: {}", parameter));
        }

        parameters[parameter] = value;
        used[parameter] = false;
    }
}

std::string Configuration::as_text(const std::string& parameter) const
{
    try
    {
        const auto key = standardize_text(parameter);
        used.at(key) = true;
        return parameters.at(key);
    }
    catch(const std::out_of_range&)
    {
        for(const auto& [key, value] : parameters)
        {
            print_key_value_pair(std::cerr, key, value);
        }
        throw std::runtime_error(std::format("Configuration parameter not found: {}", parameter));
    }
}

std::string Configuration::as_text_or_default(const std::string& parameter, const std::string& default_value) const noexcept
{
    return has_parameter(parameter) ? as_text(parameter) : String::trim_outer_whitespace(default_value);
}

bool Configuration::as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const
{
    const auto response = standardize_text(as_text(parameter));
    if(response == standardize_text(affirmative))
    {
        return true;
    }
    else if(response == standardize_text(negative))
    {
        return false;
    }
    else
    {
        throw std::runtime_error(std::format("Invalid value for \"{}\" : \"{}\"\nExpected \"{}\" or \"{}\".",
                                             parameter,
                                             as_text(parameter),
                                             affirmative,
                                             negative));
    }
}

bool Configuration::has_parameter(const std::string& parameter) const noexcept
{
    return parameters.count(standardize_text(parameter)) > 0;
}

std::string Configuration::standardize_text(const std::string& input) noexcept
{
    return String::lowercase(String::remove_extra_whitespace(input));
}

void Configuration::print_unused_parameters() const noexcept
{
    for(const auto& [param, value] : parameters)
    {
        if( ! used[param])
        {
            print_key_value_pair(std::cout, param, value);
        }
    }
}

bool Configuration::any_unused_parameters() const noexcept
{
    return std::ranges::any_of(used, [](const auto& key_value) { return ! key_value.second; });
}
