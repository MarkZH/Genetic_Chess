#include "Players/Genetic_AI.h"

#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <tuple>

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

// Sexual reproduction
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
        return legal_moves.front(); // If there's only one legal move, take it.
    }

    return std::get<Complete_Move>(search_game_tree(board,
                                                    genome.positions_to_examine(board, clock),
                                                    clock));
}

std::tuple<Complete_Move, Color, double> Genetic_AI::search_game_tree(const Board& board, double positions_to_examine, const Clock& clock) const
{
    auto perspective = board.whose_turn();
    auto legal_moves = board.all_legal_moves();
    auto best_score = -std::numeric_limits<double>::infinity();
    auto best_result = std::make_tuple(legal_moves.front(), perspective, best_score);
    auto positions_per_move = positions_to_examine/legal_moves.size();

    for(const auto& move : legal_moves)
    {
        if(clock.time_left(clock.running_for()) < 0.0)
        {
            break;
        }

        auto next_board = board;
        double score;
        try
        {
            next_board.submit_move(move);
            if(positions_per_move < 1)
            {
                score = genome.evaluate(next_board, perspective);
            }
            else
            {
                auto move_perspective_score = search_game_tree(next_board, positions_per_move, clock);
                score = std::get<double>(move_perspective_score)*(std::get<Color>(move_perspective_score) == perspective ? 1 : -1);
            }
        }
        catch(const Game_Ending_Exception&)
        {
            score = genome.evaluate(next_board, perspective);
        }

        if(score == std::numeric_limits<double>::infinity()) // checkmate lies this way
        {
            return std::make_tuple(move, perspective, score);
        }

        if(score >= best_score) // score is a number (-inf to max)
        {
            best_score = score;
            best_result = std::make_tuple(move, perspective, score);
            continue;
        }

        if(std::isnan(score) && best_score == -std::numeric_limits<double>::infinity())
        {
            best_result = std::make_tuple(move, perspective, best_score); // Stalemate is preferable only to a loss.
                                                                          // Don't change best_score since comparisons with NaN are always false.
        }
    }

    return best_result;
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
