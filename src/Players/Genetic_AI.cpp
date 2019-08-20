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

Genetic_AI::Genetic_AI(int mutation_count) : id_number(next_id++)
{
    mutate(mutation_count);
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id_number(next_id++)
{
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
