#ifndef CONFIGURATION_FILE_H
#define CONFIGURATION_FILE_H

#include <string>
#include <map>

//! A class that reads a text file to allow easy querying of configuration data.
class Configuration_File
{
    public:
        explicit Configuration_File(const std::string& file_name);

        std::string as_text(const std::string& parameter) const;
        double as_number(const std::string& parameter) const;
        bool as_boolean(const std::string& parameter, const std::string& affirmative, const std::string& negative) const;

        bool any_unused_parameters() const;
        void print_unused_parameters() const;

    private:
        std::map<std::string, std::string> parameters;
        mutable std::map<std::string, bool> used;

        static std::string standardize_text(const std::string& input);
};

#endif // CONFIGURATION_FILE_H
