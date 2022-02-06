#ifndef INTERPOLATED_GENE_VALUE_H
#define INTERPOLATED_GENE_VALUE_H

#include <array>
#include <string>
#include <map>

#include "Gene_Value.h"

enum class Game_Stage
{
    OPENING,
    ENDGAME
};

//! \brief A class for holding a value that is the interpolation of two terminal values as a game progresses.
class Interpolated_Gene_Value
{
    public:
        //! \brief Create a pair of values to interpolate between.
        //!
        //! \param name The name of this value for reading and writing to files.
        //! \param game_start_value The value to return when the game starts.
        //! \param game_end_value The value to return when the is near over.
        Interpolated_Gene_Value(const std::string& name, double game_start_value, double game_end_value) noexcept;

        //! \brief Interpolate between the two terminal values.
        //!
        //! \param game_progress How far along the number line (in the range [0,1]) to move between the start and end values.
        //! \returns A value that is linearly interpolated between the two terminal values.
        //!
        //! Mathematically, return s + (e-s)*t where s is the start, e is the end, and t is the interpolation parameter.
        double interpolate(double game_progress) const noexcept;

        //! \brief Get the start value.
        double opening_value() const noexcept;

        //! \brief Get a modifiable start value.
        double& opening_value() noexcept;

        //! \brief Get the end value.
        double endgame_value() const noexcept;

        //! \brief Get a modifiable end value.
        double& endgame_value() noexcept;

        //! \brief Get the value at a given stage of the game.
        double value_at(Game_Stage stage) const noexcept;

        //! \brief Get a modifiable value at a given stage of the game.
        double& value_at(Game_Stage stage) noexcept;

        //! \brief Create a name for this value at different game stages for reading and writing files.
        std::string name(Game_Stage stage) const noexcept;

        //! \brief Read values from a std::map that was read from a genome file.
        void load_from_map(const std::map<std::string, std::string>& properties);

        //! \brief Write values to a std::map that will write a genome file.
        void write_to_map(std::map<std::string, std::string>& properties) const noexcept;

        //! \brief Randomly mutate one of the values by the given amount.
        void mutate(double mutation_amount) noexcept;

    private:
        std::array<Gene_Value<double>, 2> values;
};

#endif // INTERPOLATED_GENE_VALUE_H
