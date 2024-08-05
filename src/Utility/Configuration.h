#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include <stdexcept>

#include "Utility/String.h"

//! \brief A class that reads a text file to allow easy querying of configuration data.
class Configuration
{
    public:
        //! \brief Read in a text file and organize the configuration data.
        //!
        //! \param file_name The name of the text file to be read.
        //!
        //! The file is read line-by-line and expects all configuration data in
        //! the form \<paramter\> = \<value\>. Paramter names are case-insensitive.
        //!
        //! Comments can be added to the file by prepending them with '#'.
        explicit Configuration(const std::string& file_name);

        //! \brief Return string data as found in the file.
        //!
        //! \param parameter The configuration parameter sought.
        //! \returns The string data with leading and trailing whitespace removed.
        //! \exception std::runtime_error If the named parameter was not found in the file.
        std::string as_text(const std::string& parameter) const;

        //! \brief Return string data if found in file. Retrun an optional value otherwise.
        //!
        //! \param parameter The configuration parameter sought.
        //! \param default_value The value to be returned if the parameter is not found.
        //! \returns The string data from the file or a default value.
        std::string as_text_or_default(const std::string& parameter, const std::string& default_value) const noexcept;

        //! \brief Return numerical data from the configuration file.
        //!
        //! \param parameter The configuration parameter sought.
        //! \tparam Number The numerical type to be returned.
        //! \returns The data in the file converted to a floating point (double) number.
        //! \exception std::runtime_error If the named parameter was not found in the file or
        //!         if the data could not be converted to a numerical value.
        template<typename Number>
        Number as_number(const std::string& parameter) const
        {
            return String::to_number<Number>(as_text(parameter));
        }

        //! \brief Return numerical data from the configuration file if it is greater than zero.
        //!
        //! \param parameter The configuration parameter sought.
        //! \tparam Number The numerical type to be returned.
        //! \returns The data in the file converted to a floating point (double) number.
        //! \exception std::runtime_error If the named parameter was not found in the file or
        //!         if the data could not be converted to a numerical value or if the
        //!         numerical value is not greater than zero.
        template<typename Number>
        Number as_positive_number(const std::string& parameter) const
        {
            auto result = as_number<Number>(parameter);
            if(result > Number{0})
            {
                return result;
            }
            else
            {
                throw std::runtime_error(parameter + " must be greater than zero (value = " + std::to_string(result) + ")");
            }
        }

        //! \brief Returns a std::chrono::duration from numerical parameter data value
        //!
        //! \param parameter The configuration parameter sought.
        //! \tparam Duration The specific std::chrono::duration type to be returned.
        //! \returns A std::chrono::duration instance that represents the textual parameter data.
        //! \exception std::runtime_error If the parameter could not be found or if the conversion
        //!         to a number or duration failed
        template<typename Duration>
        Duration as_time_duration(const std::string& parameter) const
        {
            return String::to_duration<Duration>(as_text(parameter));
        }

        //! \brief Returns a time duration as a std::chrono::duration type if it is greater than zero.
        //!
        //! \param parameter The configuration parameter sought.
        //! \tparam Duration The type of the std::chrono::duration instance.
        //! \returns A std::chrono::duration instance that represents the textual parameter data.
        //! \exception std::runtime_error If the parameter could not be found, if the conversion
        //!         to a number or duration failed, or if the resulting duration is negative.
        template<typename Duration>
        Duration as_positive_time_duration(const std::string& parameter) const
        {
            return Duration{as_positive_number<typename Duration::rep>(parameter)};
        }

        //! \brief Return true/false data from the configuration file.
        //!
        //! \param parameter The configuration parameter sought.
        //! \param affirmative The case-insensitive value corresponding to true.
        //! \param negative The case-insensitive value corresponding to false.
        //! \returns True if the data matches affirmative, false if it matches negative.
        //! \exception std::runtime_error If the named parameter was not found in the file or
        //!         if the data does not match affirmative or negative.
        bool as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const;

        //! \brief Check if the Configuration has the given parameter.
        //!
        //! \param parameter The parameter that may or may not appear in the Configuration.
        bool has_parameter(const std::string& parameter) const noexcept;

        //! \brief Check if any parameters in the configuration file were unused.
        bool any_unused_parameters() const noexcept;

        //! \brief Print the unused parameters in the configuration file to stdout.
        void print_unused_parameters() const noexcept;

    private:
        std::map<std::string, std::string> parameters;
        mutable std::map<std::string, bool> used;

        static std::string standardize_text(const std::string& input) noexcept;
};

#endif // CONFIGURATION_H
