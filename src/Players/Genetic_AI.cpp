#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

class Board;
class Clock;

#include "Game/Color.h"

#include "Utility.h"

int Genetic_AI::next_id = 0;

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
    next_id = std::max(next_id, id_number + 1);

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
            genome.read_from(ifs);

            calibrate_thinking_speed();
            calculate_centipawn_value();

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

    next_id = std::max(next_id, id_number + 1);

    genome.read_from(is);
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
