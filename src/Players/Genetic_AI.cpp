#include "Players/Genetic_AI.h"

#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>

#include "Utility.h"
#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Pieces/Piece.h"

#include "Exceptions/Checkmate_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"
#include "Exceptions/End_Of_File_Exception.h"

int Genetic_AI::next_id = 0;

Genetic_AI::Genetic_AI() :
    genome(),
    id(next_id++)
{
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id(next_id++)
{
}

Genetic_AI::~Genetic_AI()
{
}

Genetic_AI::Genetic_AI(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not read: " + file_name);
    }

    read_from(ifs);
}

Genetic_AI::Genetic_AI(std::istream& is)
{
    read_from(is);
}

Genetic_AI::Genetic_AI(const std::string& file_name, int id_in) : id(id_in)
{
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
        if(id > -1)
        {
            throw std::runtime_error("Incomplete Genetic_AI spec in file for ID " + std::to_string(id));
        }
        else
        {
            throw End_Of_File_Exception();
        }
    }

    if(id >= next_id)
    {
        next_id = id + 1;
    }

    genome.read_from(is);
}

const Complete_Move Genetic_AI::choose_move(const Board& board, const Clock& clock) const
{
    auto legal_moves = board.all_legal_moves();
    if(legal_moves.size() == 1)
    {
        return legal_moves.front();
    }

    double best_score = -std::numeric_limits<double>::infinity();
    auto best_move = legal_moves.front();
    auto positions_to_examine = genome.positions_to_examine(board, clock);
    double positions_per_move = positions_to_examine/legal_moves.size();
    for(const auto& move : legal_moves)
    {
        if(clock.time_left(clock.running_for()) < 0)
        {
            break;
        }

        auto last_board = get_final_board_state(board, move, positions_per_move, clock);
        double score = genome.evaluate(last_board, board.whose_turn());

        if(score == std::numeric_limits<double>::infinity()) // checkmate lies this way
        {
            return move;
        }
        if(score >= best_score)
        {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}

Board Genetic_AI::get_final_board_state(Board board, const Complete_Move& next_move, double positions_to_examine, const Clock& clock) const
{
    try
    {
        board.submit_move(next_move);
    }
    catch(const Game_Ending_Exception&)
    {
        return board; // game ended, no future moves
    }

    auto legal_moves = board.all_legal_moves();
    if(legal_moves.size() == 1)
    {
        return get_final_board_state(board, legal_moves.front(), positions_to_examine, clock); // only one move, no analysis of this state required
    }

    if(positions_to_examine < 1 || clock.time_left(clock.running_for()) < 0.0)
    {
        return board; // reached maximum look-ahead
    }

    double best_score = -std::numeric_limits<double>::infinity();
    Board best_board;
    auto positions_per_move = positions_to_examine/legal_moves.size();
    for(const auto& move : legal_moves)
    {
        Board last_board = get_final_board_state(board, move, positions_per_move, clock);
        double score = genome.evaluate(last_board, board.whose_turn());

        if(score == std::numeric_limits<double>::infinity()) // checkmate lies this way
        {
            return last_board;
        }
        if(score >= best_score)
        {
            best_score = score;
            best_board = last_board;
        }
    }

    return best_board;
}

void Genetic_AI::mutate()
{
    genome.mutate();
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
    std::ostringstream oss;
    oss << "Genetic AI " << get_id();
    return oss.str();
}

int Genetic_AI::get_id() const
{
    return id;
}
