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
#include "Genes/Interpolated_Gene_Value.h"

#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Exceptions.h"
#include "Utility/Math.h"

std::map<std::string, std::string> Gene::list_properties() const noexcept
{
    auto properties = std::map<std::string, std::string>{};
    priorities.write_to_map(properties);
    adjust_properties(properties);
    return properties;
}

void Gene::adjust_properties(std::map<std::string, std::string>&) const noexcept
{
}

void Gene::load_properties(const std::map<std::string, std::string>& properties)
{
    if(properties.count(priorities.name(Game_Stage::OPENING)) > 0 && properties.count(priorities.name(Game_Stage::ENDGAME)) > 0)
    {
        priorities.load_from_map(properties);
    }

    load_gene_properties(properties);
}

void Gene::load_gene_properties(const std::map<std::string, std::string>&)
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
        value.clear();
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
            if(property_data == name())
            {
                continue;
            }
            else
            {
                throw_on_invalid_line(line, "Reading data for wrong gene. Gene is " + name() + ", data is for " + property_data + ".");
            }
        }

        const auto entry = properties.find(property_name);
        if(entry == properties.end())
        {
            throw_on_invalid_line(line, "Unrecognized parameter name: " + property_name);
        }
        else if( ! entry->second.empty())
        {
            throw_on_invalid_line<Duplicate_Genome_Data>(line, "Duplicate parameter: " + property_name);
        }
        else
        {
            entry->second = property_data;
        }
    }

    const auto missing_data =
        std::accumulate(properties.begin(), properties.end(), std::string{},
                        [](const auto& so_far, const auto& key_value)
                        {
                            return so_far + (key_value.second.empty() ? "\n" + key_value.first : "");
                        });
    if( ! missing_data.empty())
    {
        throw Missing_Genome_Data("Missing gene data for " + name() + ":" + missing_data);
    }

    try
    {
        load_properties(properties);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("Bad parameter value in " + name() + ": " + e.what());
    }
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
        priorities.mutate(0.005);
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
    return priorities.interpolate(game_progress)*score_board(board, perspective, depth, game_progress);
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
    return priorities.value_at(stage);
}

bool Gene::has_priority() const noexcept
{
    return list_properties().count(priorities.name(Game_Stage::OPENING)) != 0;
}

void Gene::scale_priority(const Game_Stage stage, const double k) noexcept
{
    priorities.value_at(stage) *= k;
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

void Gene::delete_priorities(std::map<std::string, std::string>& properties) const noexcept
{
    properties.erase(priorities.name(Game_Stage::OPENING));
    properties.erase(priorities.name(Game_Stage::ENDGAME));
}
