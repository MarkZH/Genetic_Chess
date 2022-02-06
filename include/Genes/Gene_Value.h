#ifndef GENE_VALUE_H
#define GENE_VALUE_H

#include <string>
#include <map>
#include <type_traits>

#include "Utility/String.h"
#include "Utility/Random.h"

//! \brief A class for holding a value that is the interpolation of two terminal values as a game progresses.
template<typename Data_Type>
class Gene_Value
{
    public:
        //! \brief Create a pair of values to interpolate between.
        //!
        //! \param name The name of this value for reading and writing to files.
        //! \param initial_value The starting value for the gene parameter.
        Gene_Value(const std::string& name, const Data_Type& initial_value) noexcept :
            current_value(initial_value),
            identifier(name)
        {
        }

        //! \brief Get the value.
        Data_Type value() const noexcept
        {
            return current_value;
        }

        //! \brief Get a modifiable value.
        Data_Type& value() noexcept
        {
            return current_value;
        }

        //! \brief Create a name for this value for reading and writing files.
        std::string name() const noexcept
        {
            return identifier;
        }

        //! \brief Read values from a std::map that was read from a genome file.
        void load_from_map(const std::map<std::string, std::string>& properties)
        {
            current_value = String::to_number<double>(properties.at(name()));
        }

        //! \brief Write numeric values to a std::map that will write a genome file.
        void write_to_map(std::map<std::string, std::string>& properties) const noexcept
        {
            write_to_map(properties, std::to_string(current_value));
        }

        //! \brief Write data to a std::map that will write a genome file.
        void write_to_map(std::map<std::string, std::string>& properties, const std::string& data) const noexcept
        {
            properties[name()] = data;
        }

        //! \brief Randomly mutate one of the values by the given amount.
        void mutate(double mutation_amount) noexcept
        {
            static_assert(std::is_same_v<Data_Type, double>, "Default mutation only valid for doubles.");
            current_value += Random::random_laplace(mutation_amount);
        }


    private:
        Data_Type current_value;
        std::string identifier;
};

#endif // GENE_VALUE_H
