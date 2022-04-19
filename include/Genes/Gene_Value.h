#ifndef GENE_VALUE_H
#define GENE_VALUE_H

#include <string>
#include <map>

//! \brief A class for holding a value that is the interpolation of two terminal values as a game progresses.
class Gene_Value
{
    public:
        //! \brief Create a pair of values to interpolate between.
        //!
        //! \param name The name of this value for reading and writing to files.
        //! \param initial_value The starting value for the gene parameter.
        //! \param mutation_size The width of the mutation probability distribution.
        Gene_Value(const std::string& name, double initial_value, double mutation_size) noexcept;

        //! \brief Get the value.
        double value() const noexcept;

        //! \brief Get a modifiable value.
        double& value() noexcept;

        //! \brief Create a name for this value for reading and writing files.
        std::string name() const noexcept;

        //! \brief Read values from a std::map that was read from a genome file.
        void load_from_map(const std::map<std::string, std::string>& properties);

        //! \brief Write numeric values to a std::map that will write a genome file.
        void write_to_map(std::map<std::string, std::string>& properties) const noexcept;

        //! \brief Randomly mutate one of the values by the given amount.
        void mutate() noexcept;

    private:
        double current_value;
        std::string identifier;
        double mutation_amount;
};

#endif // GENE_VALUE_H
