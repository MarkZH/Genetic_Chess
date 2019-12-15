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

    std::string line;
    while(std::getline(ifs, line))
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
        used.at(parameter) = true;
        return parameters.at(standardize_text(parameter));
    }
    catch(const std::out_of_range&)
    {
        for(const auto& key_value : parameters)
        {
            std::cerr << "\"" << key_value.first << "\" --> \"" << key_value.second << "\"" << std::endl;
        }
        throw std::runtime_error("Configuration parameter not found: " + parameter);
    }
}

double Configuration::as_number(const std::string& parameter) const
{
    try
    {
        return std::stod(as_text(parameter));
    }
    catch(const std::invalid_argument&)
    {
        throw std::runtime_error("Invalid number for \"" + parameter + "\" : " + as_text(parameter));
    }
}

double Configuration::as_positive_number(const std::string& parameter) const
{
    auto result = as_number(parameter);
    if(result > 0)
    {
        return result;
    }
    else
    {
        throw std::runtime_error(parameter + " must be greater than zero (value = " + std::to_string(result) + ")");
    }
}

bool Configuration::as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const
{
    auto response = standardize_text(as_text(parameter));
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
        throw std::runtime_error("Invalid value for \"" + parameter + "\" : \"" + as_text(parameter) + "\"" +
                                 "\nExpected \"" + affirmative + "\" or \"" + negative + "\".");
    }
}

std::string Configuration::standardize_text(const std::string& input) noexcept
{
    return String::lowercase(String::remove_extra_whitespace(input));
}

void Configuration::print_unused_parameters() const noexcept
{
    for(const auto& param_value : parameters)
    {
        if( ! used[param_value.first])
        {
            std::cout << param_value.first << " --> " << param_value.second << std::endl;
        }
    }
}

bool Configuration::any_unused_parameters() const noexcept
{
    return std::any_of(used.begin(), used.end(), [](const auto& key_value) { return ! key_value.second; });
}
