#include "Players/Genetic_AI.h"

#include <iostream>
#include <array>
#include <string>
#include <cmath>

#include "Game/Color.h"

#include "Utility/String.h"
#include "Utility/Exceptions.h"

class Board;
class Clock;

namespace
{
    int next_id = 0;
}

Genetic_AI::Genetic_AI() noexcept : id_number(next_id++)
{
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) noexcept :
    genome(A.genome, B.genome),
    id_number(next_id++)
{
}

Genetic_AI::Genetic_AI(std::istream& is, const int id_in) : id_number(id_in)
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
        if( ! line.starts_with("ID"))
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
    auto add_details = [this](const auto& e)
                       {
                           return "Error in creating Genetic AI #" + std::to_string(id()) + "\n" + e.what() + "\nFile: ";
                       };
    try
    {
        genome.read_from(is);
        next_id = std::max(next_id, id() + 1);
    }
    catch(const Missing_Genome_Data& e)
    {
        throw Missing_Genome_Data(add_details(e));
    }
    catch(const Duplicate_Genome_Data& e)
    {
        throw Duplicate_Genome_Data(add_details(e));
    }
    catch(const Genetic_AI_Creation_Error& e)
    {
        throw Genetic_AI_Creation_Error(add_details(e));
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

Search_Method Genetic_AI::search_method() const noexcept
{
    return genome.search_method();
}

std::string Genetic_AI::search_method_name() const noexcept
{
    return genome.search_method_name();
}

void Genetic_AI::mutate(size_t mutation_count) noexcept
{
    for(size_t i = 0; i < mutation_count; ++i)
    {
        genome.mutate();
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
