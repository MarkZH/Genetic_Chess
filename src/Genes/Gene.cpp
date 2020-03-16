#include "Genes/Gene.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include <map>
#include <fstream>
#include <numeric>
#include <iterator>

#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/String.h"

#include "Exceptions/Genetic_AI_Creation_Error.h"

std::map<std::string, double> Gene::list_properties() const noexcept
{
    auto properties = std::map<std::string, double>{{"Priority", scoring_priority}, {"Active", double(active)}};
    adjust_properties(properties);
    return properties;
}

void Gene::adjust_properties(std::map<std::string, double>&) const noexcept
{
}

void Gene::load_properties(const std::map<std::string, double>& properties)
{
    if(properties.count("Priority") > 0)
    {
        scoring_priority = properties.at("Priority");
    }
    active = properties.at("Active") > 0.0;
    load_gene_properties(properties);
}

void Gene::load_gene_properties(const std::map<std::string, double>&)
{
}

size_t Gene::mutatable_components() const noexcept
{
    return list_properties().size();
}

void Gene::read_from(std::istream& is)
{
    auto properties = list_properties();
    std::map<std::string, bool> property_found;
    std::transform(properties.begin(), properties.end(), std::inserter(property_found, property_found.begin()),
                   [](const auto& key_value)
                   {
                       return std::make_pair(key_value.first, false);
                   });

    std::string line;
    while(std::getline(is, line))
    {
        if(String::trim_outer_whitespace(line).empty())
        {
            break;
        }
        line = String::strip_comments(line, "#");
        if(line.empty())
        {
            continue;
        }

        auto split_line = String::split(line, ":");
        if(split_line.size() != 2)
        {
            throw_on_invalid_line(line, "There should be exactly one colon per gene property line.");
        }
        auto property_name = String::remove_extra_whitespace(split_line[0]);
        auto property_data = String::remove_extra_whitespace(split_line[1]);
        if(property_name == "Name")
        {
            if(String::remove_extra_whitespace(property_data) == name())
            {
                continue;
            }
            else
            {
                throw_on_invalid_line(line, "Reading data for wrong gene. Gene is " + name() + ", data is for " + property_data + ".");
            }
        }

        try
        {
            properties.at(property_name) = std::stod(property_data);
            property_found.at(property_name) = true;
        }
        catch(const std::out_of_range&)
        {
            throw_on_invalid_line(line, "Unrecognized parameter name.");
        }
        catch(const std::invalid_argument&)
        {
            throw_on_invalid_line(line, "Bad parameter value.");
        }
    }

    auto missing_data = std::accumulate(property_found.begin(), property_found.end(), std::string{},
                        [](const auto& so_far, const auto& name_found)
                        {
                            return so_far + ( ! name_found.second ? "\n" + name_found.first : "");
                        });
    if( ! missing_data.empty())
    {
        throw Genetic_AI_Creation_Error("Missing gene data for " + name() + ":" + missing_data);
    }

    load_properties(properties);
}

void Gene::read_from(const std::string& file_name)
{
    auto ifs = std::ifstream(file_name);
    if( ! ifs)
    {
        throw Genetic_AI_Creation_Error("Could not open " + file_name + " to read.");
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        if(String::starts_with(line, "Name: "))
        {
            auto gene_name = String::remove_extra_whitespace(String::split(line, ":", 1)[1]);
            if(gene_name == name())
            {
                try
                {
                    read_from(ifs);
                    return;
                }
                catch(const std::exception& e)
                {
                    throw Genetic_AI_Creation_Error("Error in reading data for " + name() + " from " + file_name + "\n" + e.what());
                }
            }
        }
    }

    throw Genetic_AI_Creation_Error(name() + " not found in " + file_name);
}


void Gene::throw_on_invalid_line(const std::string& line, const std::string& reason) const
{
    throw Genetic_AI_Creation_Error("Invalid line in while reading for " + name() + ": " + line + "\n" + reason);
}

void Gene::mutate() noexcept
{
    if(Random::success_probability(1, 1000))
    {
        active = ! active;
    }

    if( ! is_active())
    {
        return;
    }

    auto properties = list_properties();
    if(Random::success_probability(properties.count("Priority"), properties.size()))
    {
        scoring_priority += Random::random_laplace(0.005);
    }
    else
    {
        gene_specific_mutation();
    }
}

void Gene::gene_specific_mutation() noexcept
{
}

double Gene::evaluate(const Board& board, Piece_Color perspective, size_t depth) const noexcept
{
    if(is_active())
    {
        return scoring_priority*score_board(board, perspective, depth);
    }
    else
    {
        return 0.0;
    }
}

void Gene::print(std::ostream& os) const noexcept
{
    auto properties = list_properties();
    os << "Name: " << name() << "\n";
    for(const auto& [name, value] : properties)
    {
        os << name << ": " << value << "\n";
    }
    os << "\n";
}

void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*) noexcept
{
}

double Gene::priority() const noexcept
{
    return scoring_priority;
}

void Gene::scale_priority(double k) noexcept
{
    scoring_priority *= k;
}

void Gene::zero_out_priority() noexcept
{
    scoring_priority = 0.0;
}

bool Gene::is_active() const noexcept
{
    return active;
}

void Gene::test(bool& test_variable, const Board& board, Piece_Color perspective, double expected_score) const noexcept
{
    auto result = score_board(board, perspective, board.game_length() == 0 ? 0 : 1);
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << "Error in " << name() << ": Expected " << expected_score << ", Got: " << result << '\n';
        test_variable = false;
    }
}
