#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>

//! A class that reads a text file to allow easy querying of configuration data.
class Configuration
{
    public:
        //! Read in a text file and organize the configuration data.
        //
        //! \param file_name The name of the text file to be read.
        //!
        //! The file is read line-by-line and expects all configuration data in
        //! the form \<paramter\> = \<value\>. Paramter names are case-insensitive.
        //!
        //! Comments can be added to the file by prepending them with '#'.
        explicit Configuration(const std::string& file_name);

        //! Return string data as found in the file.
        //
        //! \param parameter The configuration parameter sought.
        //! \returns The string data with leading and trailing whitespace removed.
        //! \throws std::runtime_error If the named parameter was not found in the file.
        std::string as_text(const std::string& parameter) const;

        //! Return numerical data from the configuration file.
        //
        //! \param parameter The configuration parameter sought.
        //! \returns The data in the file converted to a floating point (double) number.
        //! \throws std::runtime_error If the named parameter was not found in the file or
        //!         if the data could not be converted to a numerical value.
        double as_number(const std::string& parameter) const;

        //! Return numerical data from the configuration file if it is greater than zero.
        //
        //! \param parameter The configuration parameter sought.
        //! \returns The data in the file converted to a floating point (double) number.
        //! \throws std::runtime_error If the named parameter was not found in the file or
        //!         if the data could not be converted to a numerical value or if the
        //!         numerical value is not greater than zero.
        double as_positive_number(const std::string& parameter) const;

        //! Return true/false data from the configuration file.
        //
        //! \param parameter The configuration parameter sought.
        //! \param affirmative The case-insensitive value corresponding to true.
        //! \param negative The case-insensitive value corresponding to false.
        //! \returns True if the data matches affirmative, false if it matches negative.
        //! \throws std::runtime_error If the named parameter was not found in the file or
        //!         if the data does not match affirmative or negative.
        bool as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const;

        //! Check if any parameters in the configuration file were unused.
        bool any_unused_parameters() const noexcept;

        //! Print the unused parameters in the configuration file to stdout.
        void print_unused_parameters() const noexcept;

    private:
        std::map<std::string, std::string> parameters;
        mutable std::map<std::string, bool> used;

        static std::string standardize_text(const std::string& input) noexcept;
};

#endif // CONFIGURATION_H