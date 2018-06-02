#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Game/Color.h"

#include "Utility.h"

int Genetic_AI::next_id = 0;

Genetic_AI::Genetic_AI() :
    genome(),
    id(next_id++)
{
    calibrate_thinking_speed();
    calculate_centipawn_value();
}

// Sexual reproduction
Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id(next_id++)
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

Genetic_AI::Genetic_AI(const std::string& file_name, int id_in) : id(id_in)
{
    if(id >= next_id)
    {
        next_id = id + 1;
    }

    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not read: " + file_name);
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        if( ! String::starts_with(line, "ID:"))
        {
            continue;
        }

        auto param_value = String::split(line, ":", 1);
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
    id = -1;
    while(std::getline(is, line))
    {
        if(line.empty())
        {
            continue;
        }

        if(String::starts_with(line, "ID:"))
        {
            id = std::stoi(String::split(line)[1]);
            break;
        }
        else
        {
            throw std::runtime_error("Invalid Genetic_AI line: " + line);
        }
    }

    if( ! is)
    {
        throw std::runtime_error("Incomplete Genetic_AI spec in file for ID " + std::to_string(id));
    }

    if(id >= next_id)
    {
        next_id = id + 1;
    }

    genome.read_from(is);
}

double Genetic_AI::evaluate(const Board & board, Game_Result move_result, Color perspective) const
{
    return genome.evaluate(board, move_result, perspective);
}

double Genetic_AI::time_to_examine(const Board & board, const Clock & clock) const
{
    return genome.time_to_examine(board, clock);
}

double Genetic_AI::speculation_time_factor(const Board & board) const
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

void Genetic_AI::print_genome(const std::string& file_name) const
{
    if(file_name.empty())
    {
        print_genome(std::cout);
    }
    else
    {
        auto dest = std::ofstream(file_name, std::ofstream::app);
        print_genome(dest);
    }
}

void Genetic_AI::print_genome(std::ostream& os) const
{
    os << "ID: " << get_id() << std::endl;
    genome.print(os);
    os << "END" << std::endl << std::endl;
}

std::string Genetic_AI::name() const
{
    return "Genetic AI " + std::to_string(get_id());
}

std::string Genetic_AI::author() const
{
    return "Mark Harrison";
}

int Genetic_AI::get_id() const
{
    return id;
}

bool Genetic_AI::operator<(const Genetic_AI& other) const
{
    return get_id() < other.get_id();
}

bool Genetic_AI::operator==(const Genetic_AI& other) const
{
    return get_id() == other.get_id();
}
