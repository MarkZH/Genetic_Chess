#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

class Board;
class Clock;

#include "Game/Color.h"

#include "Utility/String.h"

#include "Exceptions/Genetic_AI_Creation_Error.h"

int Genetic_AI::next_id = 0;
size_t Genetic_AI::max_origin_pool_id = 0;

Genetic_AI::Genetic_AI(int mutation_count) :
    id_number(next_id++)
{
    mutate(mutation_count);
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id_number(next_id++)
{
    auto A_parents = A.ancestry;
    auto B_parents = B.ancestry;
    for(size_t i = 0; i <= max_origin_pool_id; ++i)
    {
        ancestry[i] = (A_parents[i] + B_parents[i])/2;
    }

    recalibrate_self();
}

Genetic_AI::Genetic_AI(const std::string& file_name, int id_in) : id_number(id_in)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw Genetic_AI_Creation_Error("Could not read: " + file_name);
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        line = String::strip_comments(line, "#");
        if( ! String::starts_with(line, "ID"))
        {
            continue;
        }

        auto param_value = String::split(line, ":", 1);
        if(param_value.size() != 2 || String::trim_outer_whitespace(param_value[0]) != "ID")
        {
            continue;
        }

        if(id_in == std::stoi(param_value[1]))
        {
            read_data(ifs);
            return;
        }
    }

    throw Genetic_AI_Creation_Error("Could not locate ID " + std::to_string(id_in) + " inside file " + file_name);
}

void Genetic_AI::read_data(std::istream& is)
{
    try
    {
        read_ancestry(is);
        genome.read_from(is);
        recalibrate_self();
        next_id = std::max(next_id, id() + 1);
    }
    catch(const Genetic_AI_Creation_Error& e)
    {
        throw Genetic_AI_Creation_Error("Error in creating Genetic AI #" + std::to_string(id()) + "\n" + e.what());
    }
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

    recalibrate_self();
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
    os << "Name: Ancestry\n";
    for(size_t i = 0; i <= max_origin_pool_id; ++i)
    {
        auto fraction = (ancestry.count(i) > 0 ? ancestry.at(i) : 0.0);
        os << i << ": " << fraction << "\n";
    }
    os << "\n";
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

void Genetic_AI::set_origin_pool(size_t pool_id)
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
        line = String::strip_comments(line, "#");
        if(line.empty())
        {
            continue;
        }

        if(String::starts_with(line, "Name"))
        {
            auto data = String::split(line, ":", 1);
            auto header = String::trim_outer_whitespace(data.front());
            if(data.size() != 2 || header != "Name")
            {
                throw Genetic_AI_Creation_Error("Bad section header (" + id_string + "): " + line);
            }
            auto section = String::remove_extra_whitespace(data.back());
            if(section == "Ancestry")
            {
                break;
            }
            else
            {
                throw Genetic_AI_Creation_Error("Missing ancestry data " + id_string);
            }
        }
    }

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

        auto pool_fraction = String::split(line, ":");
        if(pool_fraction.size() != 2)
        {
            throw Genetic_AI_Creation_Error("Bad ancestry line " + id_string + ": " + line);
        }

        try
        {
            auto pool = String::string_to_size_t(pool_fraction[0]);
            auto fraction = std::stod(pool_fraction[1]);
            ancestry[pool] = fraction;
            max_origin_pool_id = std::max(max_origin_pool_id, pool);
        }
        catch(const std::exception&)
        {
            throw Genetic_AI_Creation_Error("Bad ancesry line " + id_string + ": " + line);
        }
    }
}
