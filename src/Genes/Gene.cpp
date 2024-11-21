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
#include <string>
#include <format>

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
    gene_turn_on_progress.write_to_map(properties);
    gene_turn_off_progress.write_to_map(properties);
    adjust_properties(properties);
    return properties;
}

void Gene::adjust_properties(std::map<std::string, std::string>&) const noexcept
{
}

void Gene::load_properties(const std::map<std::string, std::string>& properties)
{
    if(has_priority())
    {
        priorities.load_from_map(properties);
    }

    if(has_activation())
    {
        gene_turn_on_progress.load_from_map(properties);
        gene_turn_off_progress.load_from_map(properties);
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

Gene::Gene(const std::string& name_of_gene) noexcept : gene_name(name_of_gene)
{
}

std::string Gene::name() const noexcept
{
    return gene_name;
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
                throw_on_invalid_line(line, std::format("Reading data for wrong gene. Gene is {}, data is for {}.", name(), property_data));
            }
        }

        const auto entry = properties.find(property_name);
        if(entry == properties.end())
        {
            throw_on_invalid_line(line, std::format("Unrecognized parameter name: {}", property_name));
        }
        else if( ! entry->second.empty())
        {
            throw_on_invalid_line<Duplicate_Genome_Data>(line, std::format("Duplicate parameter: {}", property_name));
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
                            if(key_value.second.empty())
                            {
                                return std::format("{}\n{}", so_far, key_value.first);
                            }
                            else
                            {
                                return so_far;
                            }
                        });
    if( ! missing_data.empty())
    {
        throw Missing_Genome_Data(std::format("Missing gene data for {}:{}", name(), missing_data));
    }

    try
    {
        load_properties(properties);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(std::format("Bad parameter value in {}: {}", name(), e.what()));
    }
}

void Gene::read_from(const std::string& file_name)
{
    auto ifs = std::ifstream(file_name);
    if( ! ifs)
    {
        throw Genome_Creation_Error(std::format("Could not open {} to read.", file_name));
    }

    for(std::string line; std::getline(ifs, line);)
    {
        if(line.starts_with("Name: "))
        {
            const auto read_gene_name = String::remove_extra_whitespace(String::split(line, ":", 1)[1]);
            if(read_gene_name == name())
            {
                auto add_details = [this, &file_name](const auto& e)
                                   {
                                       return std::format("Error in reading data for {} from {}\n{}", name(), file_name, e.what());
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

    throw Genome_Creation_Error(std::format("{} not found in {}", name(), file_name));
}

void Gene::mutate() noexcept
{
    const auto properties = list_properties();
    const auto priority_count = has_priority() ? 2 : 0;
    const auto activation_count = has_activation() ? 2 : 0;
    if(Random::success_probability(priority_count, properties.size()))
    {
        priorities.mutate();
    }
    else if(Random::success_probability(activation_count, properties.size() - priority_count))
    {
        mutate_activations();
    }
    else
    {
        gene_specific_mutation();
    }
}

void Gene::mutate_activations() noexcept
{
    const auto mutate_turn_on = Random::coin_flip();
    auto& mutating_value = mutate_turn_on ? gene_turn_on_progress : gene_turn_off_progress;
    auto& clamping_value = mutate_turn_on ? gene_turn_off_progress : gene_turn_on_progress;
    mutating_value.mutate();

    // The end margin makes it easier for this gene value to end up activating the gene for the
    // first and/or last moves. Otherwise, if the lower limit was zero and the upper limit one,
    // then half of all mutations would result in genes being turned off and the first/last moves.
    const auto end_margin = 0.05;
    const auto lower_limit = 0.0 - end_margin;
    const auto upper_limit = 1.0 + end_margin;
    mutating_value.value() = std::clamp(mutating_value.value(), lower_limit, upper_limit);

    // Allow the begin and end of activation to be in reverse order by the same margin to
    // more firmly shut off the gene.
    const auto clamping_lower_limit = mutate_turn_on ? mutating_value.value() - end_margin : lower_limit;
    const auto clampling_uper_limit = mutate_turn_on ? upper_limit : mutating_value.value() + end_margin;
    clamping_value.value() = std::clamp(clamping_value.value(), clamping_lower_limit, clampling_uper_limit);
}

void Gene::gene_specific_mutation() noexcept
{
}

double Gene::evaluate(const Board& board, const Piece_Color perspective, const size_t depth, const double game_progress) const noexcept
{
    return active(game_progress) ? priorities.interpolate(game_progress) * score_board(board, perspective, depth) : 0.0;
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

bool Gene::has_priority() const noexcept
{
    return list_properties().count(priorities.name(Game_Stage::OPENING)) != 0;
}

bool Gene::has_activation() const noexcept
{
    return list_properties().count(gene_turn_on_progress.name()) != 0;
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

    const auto result = score_board(board, perspective, board.played_ply_count());
    if(std::abs(result - expected_score) > 1e-6)
    {
        std::cerr << std::format("Error in {} Test #{}: Expected {}, Got: {}\n", name(), test_number, expected_score, result);
        board.cli_print(std::cerr);
        test_variable = false;
    }
}

void Gene::delete_priorities(std::map<std::string, std::string>& properties) const noexcept
{
    properties.erase(priorities.name(Game_Stage::OPENING));
    properties.erase(priorities.name(Game_Stage::ENDGAME));
}

void Gene::delete_activations(std::map<std::string, std::string>& properties) const noexcept
{
    properties.erase(gene_turn_on_progress.name());
    properties.erase(gene_turn_off_progress.name());
}

bool Gene::active(const double game_progress) const noexcept
{
    return gene_turn_on_progress.value() <= game_progress && game_progress <= gene_turn_off_progress.value();
}
