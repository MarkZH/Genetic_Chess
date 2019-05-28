#include "Utility/Configuration_File.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>

#include "Utility/String.h"

//! Read in a text file and organize the configuration data.
//
//! \param file_name The name of the text file to be read.
//!
//! The file is read line-by-line and expects all configuration data in
//! the form \<paramter\> = \<value\>. Paramters are case-insensitive, as
//! are values except for file names.
//!
//! Comments can be added to the file by prepending them with '#'.
Configuration_File::Configuration_File(const std::string& file_name)
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
        parameters[parameter] = value;
        used[parameter] = false;
    }
}

//! Return string data as found in the file.
//
//! \param parameter The configuration parameter sought.
//! \returns The string data with leading and trailing whitespace removed.
//! \throws std::runtime_error If the named parameter was not found in the file.
std::string Configuration_File::as_text(const std::string& parameter) const
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

//! Return numerical data from the configuration file.
//
//! \param parameter The configuration parameter sought.
//! \returns The data in the file converted to a floating point (double) number.
//! \throws std::runtime_error If the named parameter was not found in the file or
//!         if the data could not be converted to a numerical value.
double Configuration_File::as_number(const std::string& parameter) const
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

//! Return true/false data from the configuration file.
//
//! \param parameter The configuration parameter sought.
//! \param affirmative The case-insensitive value corresponding to true.
//! \param negative The case-insensitive value corresponding to false.
//! \returns True if the data matches affirmative, false if it matches negative.
//! \throws std::runtime_error If the named parameter was not found in the file or
//!         if the data does not match affirmative or negative.
bool Configuration_File::as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const
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

std::string Configuration_File::standardize_text(const std::string& input)
{
    return String::lowercase(String::remove_extra_whitespace(input));
}

//! Print the unused parameters in the configuration file to stdout.
void Configuration_File::print_unused_parameters() const
{
    for(const auto& param_value : parameters)
    {
        if( ! used[param_value.first])
        {
            std::cout << param_value.first << " --> " << param_value.second << std::endl;
        }
    }
}

//! Check if any parameters in the configuration file were unused.
bool Configuration_File::any_unused_parameters() const
{
    return std::any_of(used.begin(), used.end(), [](const auto& key_value) { return ! key_value.second; });
}
