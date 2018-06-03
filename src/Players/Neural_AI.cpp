#include "Players/Neural_AI.h"

#include <iostream>
#include <fstream>

#include "Game/Board.h"
#include "Game/Clock.h"

size_t Neural_AI::next_id = 0;

Neural_AI::Neural_AI() : id(next_id++)
{
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

void Neural_AI::mutate(size_t iterations)
{
    brain.mutate(iterations);
}

void Neural_AI::print(const std::string & file_name) const
{
    print(std::ofstream(file_name, std::ios::app));
}

void Neural_AI::print(std::ostream & output) const
{
    output << "ID: " << get_id() << '\n';
    brain.print(output);
}

double Neural_AI::internal_evaluate(const Board& board, Color perspective) const
{
    return brain.evaluate(board, perspective);
}

double Neural_AI::speculation_time_factor(const Board& board) const
{
    return 1.0;
}

double Neural_AI::time_to_examine(const Board&, const Clock& clock) const
{
    return clock.time_left(clock.running_for())/30.0;
}
