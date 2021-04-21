#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <cmath>

#include "Game/Color.h"

#include "Utility/String.h"
#include "Utility/Exceptions.h"

class Board;
class Clock;

int Genetic_AI::next_id = 0;

Genetic_AI::Genetic_AI() noexcept : id_number(next_id++)
{
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) noexcept :
    genome(A.genome, B.genome),
    id_number(next_id++)
{
    recalibrate_self();
}

Genetic_AI::Genetic_AI(const std::string& file_name, const int id_in) try : Genetic_AI(std::ifstream(file_name), id_in)
{
}
catch(const Genetic_AI_Creation_Error& e)
{
    throw Genetic_AI_Creation_Error(e.what() + file_name);
}

Genetic_AI::Genetic_AI(std::istream& is, const int id_in) : id_number(id_in)
{
    read_from(is);
}

Genetic_AI::Genetic_AI(std::istream&& is, const int id_in) : id_number(id_in)
{
    read_from(is);
}

void Genetic_AI::read_from(std::istream& is)
{
    if( ! is)
    {
        throw Genetic_AI_Creation_Error("Could not read: ");
    }

    for(std::string line; std::getline(is, line);)
    {
        line = String::strip_comments(line, "#");
        if( ! String::starts_with(line, "ID"))
        {
            continue;
        }

        const auto param_value = String::split(line, ":", 1);
        if(param_value.size() != 2 || String::trim_outer_whitespace(param_value[0]) != "ID")
        {
            continue;
        }

        if(id_number == String::to_number<int>(param_value[1]))
        {
            read_data(is);
            return;
        }
    }

    throw Genetic_AI_Creation_Error("Could not locate ID " + std::to_string(id_number) + " inside file ");
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
        throw Genetic_AI_Creation_Error("Error in creating Genetic AI #" + std::to_string(id()) + "\n" + e.what() + "\nFile: ");
    }
}

double Genetic_AI::internal_evaluate(const Board& board, Piece_Color perspective, size_t depth) const noexcept
{
    return genome.evaluate(board, perspective, depth);
}

const std::array<double, 6>& Genetic_AI::piece_values() const noexcept
{
    return genome.piece_values();
}

Clock::seconds Genetic_AI::time_to_examine(const Board& board, const Clock& clock) const noexcept
{
    return genome.time_to_examine(board, clock);
}

double Genetic_AI::speculation_time_factor(double game_progress) const noexcept
{
    return genome.speculation_time_factor(game_progress);
}

double Genetic_AI::branching_factor(double game_progress) const noexcept
{
    return genome.branching_factor(game_progress);
}

double Genetic_AI::game_progress(const Board& board) const noexcept
{
    return genome.game_progress(board);
}

void Genetic_AI::mutate(size_t mutation_count) noexcept
{
    for(size_t i = 0; i < mutation_count; ++i)
    {
        genome.mutate();
    }

    recalibrate_self();
}

void Genetic_AI::print(const std::string& file_name) const noexcept
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

void Genetic_AI::print(std::ostream& os) const noexcept
{
    os << "ID: " << id() << '\n';
    genome.print(os);
    os << "END\n" << std::endl;
}

std::string Genetic_AI::name() const noexcept
{
    return "Genetic AI " + std::to_string(id());
}

std::string Genetic_AI::author() const noexcept
{
    return "Mark Harrison";
}

int Genetic_AI::id() const noexcept
{
    return id_number;
}

bool Genetic_AI::operator<(const Genetic_AI& other) const noexcept
{
    return id() < other.id();
}

int find_last_id(const std::string& players_file_name)
{
    std::ifstream player_input(players_file_name);
    if( ! player_input)
    {
        throw std::invalid_argument("File not found: " + players_file_name);
    }

    std::string last_player;
    for(std::string line; std::getline(player_input, line);)
    {
        if(String::starts_with(line, "ID:"))
        {
            last_player = line;
        }
    }

    if(last_player.empty())
    {
        throw std::runtime_error("No valid ID found in file: " + players_file_name);
    }

    const auto split = String::split(last_player, ":", 1);
    if(split.size() != 2)
    {
        throw std::runtime_error("Invalid ID line: " + last_player);
    }

    try
    {
        return String::to_number<int>(split.back());
    }
    catch(const std::exception&)
    {
        throw std::runtime_error("Could not convert to ID number: " + last_player);
    }
}
