#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <algorithm>

class Board;
class Clock;

#include "Game/Color.h"

#include "Utility.h"

int Genetic_AI::next_id = 0;
int Genetic_AI::max_origin_pool_id = 0;

Genetic_AI::Genetic_AI() :
    genome(),
    id_number(next_id++)
{
    calibrate_thinking_speed();
    calculate_centipawn_value();
}

// Sexual reproduction
Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id_number(next_id++)
{
    auto A_parents = A.ancestry;
    auto B_parents = B.ancestry;
    for(auto i = 0; i <= max_origin_pool_id; ++i)
    {
        ancestry[i] = (A_parents[i] + B_parents[i])/2;
    }

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

Genetic_AI::Genetic_AI(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not read: " + file_name);
    }

    read_from(ifs);

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

Genetic_AI::Genetic_AI(std::istream& is)
{
    read_from(is);

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

Genetic_AI::Genetic_AI(const std::string& file_name, int id_in) : id_number(id_in)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not read: " + file_name);
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        line = String::strip_comments(line, '#');
        if( ! String::starts_with(line, "ID"))
        {
            continue;
        }

        auto param_value = String::split(line, ":", 1);
        if(param_value.size() != 2 || String::trim_outer_whitespace(param_value[0]) != "ID")
        {
            throw std::runtime_error("Bad ID line: " + line);
        }

        if(id_in == std::stoi(param_value[1]))
        {
            read_ancestry(ifs);
            genome.read_from(ifs);

            calibrate_thinking_speed();
            calculate_centipawn_value();

            next_id = std::max(next_id, id_number + 1);

            return;
        }
    }

    throw std::runtime_error("Could not locate ID " + std::to_string(id_in) + " inside file " + file_name);
}

void Genetic_AI::read_from(std::istream& is)
{
    std::string line;
    id_number = -1;
    while(std::getline(is, line))
    {
        line = String::strip_comments(line, '#');
        if(line.empty())
        {
            continue;
        }

        if(String::starts_with(line, "ID"))
        {
            auto param_value = String::split(line, ":", 1);
            if(param_value.size() != 2 || String::trim_outer_whitespace(param_value[0]) != "ID")
            {
                throw std::runtime_error("Bad ID line: " + line);
            }
            id_number = std::stoi(param_value[1]);
            break;
        }
        else
        {
            throw std::runtime_error("Invalid Genetic_AI line: " + line);
        }
    }

    if( ! is)
    {
        throw std::runtime_error("Incomplete Genetic_AI spec in file for ID " + std::to_string(id_number));
    }

    read_ancestry(is);
    genome.read_from(is);

    next_id = std::max(next_id, id_number + 1);
}

double Genetic_AI::internal_evaluate(const Board& board, Color perspective, size_t depth) const
{
    return genome.evaluate(board, perspective, depth);
}

double Genetic_AI::time_to_examine(const Board& board, const Clock& clock) const
{
    return genome.time_to_examine(board, clock);
}

double Genetic_AI::speculation_time_factor(const Board& board) const
{
    return genome.speculation_time_factor(board);
}

void Genetic_AI::mutate(int mutation_count)
{
    for(int i = 0; i < mutation_count; ++i)
    {
        genome.mutate();
    }

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

void Genetic_AI::print(const std::string& file_name) const
{
    if(file_name.empty())
    {
        print(std::cout);
    }
    else
    {
        auto dest = std::ofstream(file_name, std::ofstream::app);
        print(dest);
    }
}

void Genetic_AI::print(std::ostream& os) const
{
    os << "ID: " << id() << '\n';
    os << "Ancestry: ";
    for(auto i = 0; i <= max_origin_pool_id; ++i)
    {
        auto fraction = (ancestry.count(i) > 0 ? ancestry.at(i) : 0.0);
        os << i << " -> " << fraction << " / ";
    }
    os << "\n\n";
    genome.print(os);
    os << "END" << "\n" << std::endl;
}

std::string Genetic_AI::name() const
{
    return "Genetic AI " + std::to_string(id());
}

std::string Genetic_AI::author() const
{
    return "Mark Harrison";
}

int Genetic_AI::id() const
{
    return id_number;
}

bool Genetic_AI::operator<(const Genetic_AI& other) const
{
    return id() < other.id();
}

bool Genetic_AI::operator==(const Genetic_AI& other) const
{
    return id() == other.id();
}

void Genetic_AI::set_origin_pool(int pool_id)
{
    ancestry.clear();
    ancestry[pool_id] = 1.0;
    max_origin_pool_id = std::max(max_origin_pool_id, pool_id);
}

void Genetic_AI::read_ancestry(std::istream& is)
{
    auto id_string = "(ID: " + std::to_string(id()) + ")";

    std::string line;
    while(std::getline(is, line))
    {
        line = String::strip_comments(line, '#');
        if(line.empty())
        {
            continue;
        }

        if( ! String::starts_with(line, "Ancestry:"))
        {
            throw std::runtime_error("Missing ancestry data " + id_string);
        }

        break;
    }

    auto line_split = String::split(line, ":");
    if(line_split.size() != 2)
    {
        throw std::runtime_error("Too many colons in ancestry line " + id_string + ": " + line);
    }

    auto family_data = line_split.back();
    for(auto family : String::split(family_data, "/"))
    {
        family = String::trim_outer_whitespace(family);
        if(family.empty())
        {
            continue;
        }

        auto pool_fraction = String::split(family, "->");
        if(pool_fraction.size() != 2)
        {
            throw std::runtime_error("Malformed ancestry line " + id_string + ": " + line);
        }

        auto pool = std::stoi(pool_fraction[0]);
        auto fraction = std::stod(pool_fraction[1]);
        ancestry[pool] = fraction;
        max_origin_pool_id = std::max(max_origin_pool_id, pool);
    }
}
