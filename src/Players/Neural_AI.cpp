#include "Players/Neural_AI.h"

#include <iostream>
#include <fstream>

#include "Game/Board.h"
#include "Game/Clock.h"

#include "Utility.h"

size_t Neural_AI::next_id = 0;

Neural_AI::Neural_AI() : id(next_id++)
{
    calibrate_thinking_speed();
    calculate_centipawn_value();
}

Neural_AI::Neural_AI(const std::string& file_name)
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

Neural_AI::Neural_AI(std::istream& is)
{
    read_from(is);

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

Neural_AI Neural_AI::copy() const
{
    Neural_AI new_copy;
    new_copy.brain = brain;
    return new_copy;
}

Neural_AI::Neural_AI(const std::string& file_name, int id_in) : id(id_in)
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
            brain.read_from(ifs);

            calibrate_thinking_speed();
            calculate_centipawn_value();

            return;
        }
    }

    throw std::runtime_error("Could not locate ID " + std::to_string(id_in) + " inside file " + file_name);
}

void Neural_AI::read_from(std::istream& is)
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
            throw std::runtime_error("Invalid Neural_AI line: " + line);
        }
    }

    if( ! is)
    {
        throw std::runtime_error("Incomplete Neural_AI spec in file for ID " + std::to_string(id));
    }

    if(id >= next_id)
    {
        next_id = id + 1;
    }

    brain.read_from(is);
}

std::string Neural_AI::name() const
{
    return "Neural Net AI";
}

std::string Neural_AI::author() const
{
    return "Mark Harrison";
}

size_t Neural_AI::get_id() const
{
    return id;
}

void Neural_AI::mutate(int iterations)
{
    brain.mutate(iterations);

    calibrate_thinking_speed();
    calculate_centipawn_value();
}

void Neural_AI::print(const std::string & file_name) const
{
    auto ofs = std::ofstream(file_name, std::ios::app);
    print(ofs);
}

void Neural_AI::print(std::ostream & output) const
{
    output << "ID: " << get_id() << '\n';
    brain.print(output);
    output << "END\n\n";
}

bool Neural_AI::operator<(const Neural_AI& other) const
{
    return get_id() < other.get_id();
}

bool Neural_AI::operator==(const Neural_AI& other) const
{
    return get_id() == other.get_id();
}

double Neural_AI::internal_evaluate(const Board& board, Color perspective) const
{
    return brain.evaluate(board, perspective);
}

double Neural_AI::speculation_time_factor(const Board&) const
{
    return 1.0;
}

double Neural_AI::time_to_examine(const Board& board, const Clock& clock) const
{
    auto moves_so_far = board.get_game_record().size()/2; // only count own moves
    return clock.time_left(clock.running_for())/Math::average_moves_left(40.0, 0.5, moves_so_far);
}
