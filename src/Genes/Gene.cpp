#include "Genes/Gene.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include <map>
#include <fstream>
#include <numeric>
#include <iterator>
#include <limits>

#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Exceptions.h"
#include "Utility/Math.h"

std::map<std::string, double> Gene::list_properties() const noexcept
{
    auto properties = std::map<std::string, double>{{"Priority - Opening", opening_priority},
                                                    {"Priority - Endgame", endgame_priority}};
    adjust_properties(properties);
    return properties;
}

void Gene::adjust_properties(std::map<std::string, double>&) const noexcept
{
}

void Gene::load_properties(const std::map<std::string, double>& properties)
{
    if(properties.count("Priority - Opening") > 0)
    {
        opening_priority = properties.at("Priority - Opening");
    }

    if(properties.count("Priority - Endgame") > 0)
    {
        endgame_priority = properties.at("Priority - Endgame");
    }

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
    for(auto& [key, value] : properties)
    {
        value = std::numeric_limits<double>::quiet_NaN();
    }

    for(std::string line; std::getline(is, line);)
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

        const auto split_line = String::split(line, ":");
        if(split_line.size() != 2)
        {
            throw_on_invalid_line(line, "There should be exactly one colon per gene property line.");
        }
        const auto property_name = String::remove_extra_whitespace(split_line[0]);
        const auto property_data = String::remove_extra_whitespace(split_line[1]);
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
            auto& current_value = properties.at(property_name);
            if(std::isnan(current_value))
            {
                current_value = String::to_number<double>(property_data);
            }
            else
            {
                throw_on_invalid_line<Duplicate_Genome_Data>(line, "Duplicate parameter: " + property_name);
            }
        }
        catch(const std::out_of_range& e)
        {
            if(String::contains(e.what(), "at") || String::contains(e.what(), "invalid map<K, T> key"))
            {
                throw_on_invalid_line(line, "Unrecognized parameter name: " + property_name);
            }
            else if(String::contains(e.what(), "stod"))
            {
                throw_on_invalid_line(line, "Bad parameter value: " + property_data);
            }
            else
            {
                throw_on_invalid_line(line, e.what());
            }
        }
        catch(const std::invalid_argument&)
        {
            throw_on_invalid_line(line, "Bad parameter value: " + property_data);
        }
        catch(const Duplicate_Genome_Data&)
        {
            throw;
        }
        catch(const std::exception& e)
        {
            throw_on_invalid_line(line, e.what());
        }
    }

    const auto missing_data =
        std::accumulate(properties.begin(), properties.end(), std::string{},
                        [](const auto& so_far, const auto& key_value)
                        {
                            return so_far + (std::isnan(key_value.second) ? "\n" + key_value.first : "");
                        });
    if( ! missing_data.empty())
    {
        throw Missing_Genome_Data("Missing gene data for " + name() + ":" + missing_data);
    }

    load_properties(properties);
}

void Gene::read_from(const std::string& file_name)
{
    auto ifs = std::ifstream(file_name);
    if( ! ifs)
    {
        throw Genome_Creation_Error("Could not open " + file_name + " to read.");
    }

    for(std::string line; std::getline(ifs, line);)
    {
        if(String::starts_with(line, "Name: "))
        {
            const auto gene_name = String::remove_extra_whitespace(String::split(line, ":", 1)[1]);
            if(gene_name == name())
            {
                auto add_details = [this, &file_name](const auto& e)
                                   {
                                       return "Error in reading data for " + name() + " from " + file_name + "\n" + e.what();
                                   };
                try
                {
                    read_from(ifs);
                    return;
                }
                catch(const Missing_Genome_Data& e)
                {
                    throw Missing_Genome_Data(add_details(e));
                }
                catch(const Duplicate_Genome_Data& e)
                {
                    throw Duplicate_Genome_Data(add_details(e));
                }
                catch(const std::exception& e)
                {
                    throw Genome_Creation_Error(add_details(e));
                }
            }
        }
    }

    throw Genome_Creation_Error(name() + " not found in " + file_name);
}

void Gene::mutate() noexcept
{
    const auto properties = list_properties();
    if(has_priority() && Random::success_probability(2, properties.size()))
    {
        (Random::coin_flip() ? opening_priority : endgame_priority) += Random::random_laplace(0.005);
    }
    else
    {
        gene_specific_mutation();
    }
}

void Gene::gene_specific_mutation() noexcept
{
}

double Gene::evaluate(const Board& board, const Piece_Color perspective, const size_t depth, const double game_progress) const noexcept
{
    const auto scoring_priority = Math::interpolate(opening_priority, endgame_priority, game_progress);
    return scoring_priority*score_board(board, perspective, depth, game_progress);
}

void Gene::print(std::ostream& os) const noexcept
{
    os << "Name: " << name() << "\n";
    for(const auto& [name, value] : list_properties())
    {
        os << name << ": " << value << "\n";
    }
    os << "\n";
}

void Gene::reset_piece_strength_gene(const Piece_Strength_Gene*) noexcept
{
}

double Gene::priority(const Game_Stage stage) const noexcept
{
    return stage == Game_Stage::OPENING ? opening_priority : endgame_priority;
}

bool Gene::has_priority() const noexcept
{
    return list_properties().count("Priority - Opening") != 0;
}

void Gene::scale_priority(const Game_Stage stage, const double k) noexcept
{
    (stage == Game_Stage::OPENING ? opening_priority : endgame_priority) *= k;
}

void Gene::test(bool& test_variable, const Board& board, const Piece_Color perspective, const double expected_score) const noexcept
{
    static auto test_number = 0;
    static auto last_gene_name = std::string{};

    if(name() != last_gene_name)
    {
        test_number = 1;
        last_gene_name = name();
    }
    else
    {
        ++test_number;
    }

    const auto result = score_board(board, perspective, board.played_ply_count(), 0.0);
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << "Error in " << name() << " Test #" << test_number << ": Expected " << expected_score << ", Got: " << result << '\n';
        test_variable = false;
    }
}
