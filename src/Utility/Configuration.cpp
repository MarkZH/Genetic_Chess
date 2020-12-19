#include "Utility/Configuration.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "Utility/String.h"

Configuration::Configuration(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not open configuration file: " + file_name);
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
            throw std::runtime_error("Configuration file lines must be of form \"Name = Value\"\n" + line);
        }
        auto line_split = String::split(line, "=", 1);
        auto parameter = standardize_text(line_split[0]);
        auto value = String::trim_outer_whitespace(line_split[1]);
        if(value.empty())
        {
            throw std::runtime_error("Configuration parameter cannot be empty.\n" + line);
        }

        if(parameters.count(parameter) > 0)
        {
            throw std::runtime_error("Configuration parameter used more than once: " + parameter);
        }
        parameters[parameter] = value;
        used[parameter] = false;
    }
}

std::string Configuration::as_text(const std::string& parameter) const
{
    try
    {
        auto key = standardize_text(parameter);
        used.at(key) = true;
        return parameters.at(key);
    }
    catch(const std::out_of_range&)
    {
        for(const auto& [key, value] : parameters)
        {
            std::cerr << "\"" << key << "\" --> \"" << value << "\"" << std::endl;
        }
        throw std::runtime_error("Configuration parameter not found: " + parameter);
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
            std::cout << param << " --> " << value << std::endl;
        }
    }
}

bool Configuration::any_unused_parameters() const noexcept
{
    return std::any_of(used.begin(), used.end(), [](const auto& key_value) { return ! key_value.second; });
}
