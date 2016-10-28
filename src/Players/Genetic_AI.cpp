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

Genetic_AI::Genetic_AI(const Genetic_AI& other, bool is_clone) :
    genome(other.genome),
    id(is_clone ? next_id++ : other.id)
{
}

Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id(next_id++)
{
}

Genetic_AI& Genetic_AI::operator=(Genetic_AI other)
{
    genome = other.genome;
    id = other.id;

    return *this;
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
    return choose_move(board, clock, -1);
}

const Complete_Move Genetic_AI::choose_move(const Board& board, const Clock& clock, int positions_to_examine) const
{
    auto legal_moves = board.all_legal_moves();
    if(legal_moves.size() == 1)
    {
        return legal_moves.front();
    }
    double best_score = std::numeric_limits<double>::lowest();
    auto best_move = legal_moves.front();
    if(positions_to_examine == -1)
    {
        positions_to_examine = genome.positions_to_examine(board, clock);
    }
    int positions_per_move = std::ceil(double(positions_to_examine)/legal_moves.size());

    for(const auto& move : legal_moves)
    {
        if(clock.time_left(clock.running_for()) < std::min(0.0, genome.time_required()))
        {
            break;
        }

        double score;
        try
        {
            if(positions_per_move > positions_to_examine)
            {
                positions_per_move = positions_to_examine;
            }
            positions_to_examine -= positions_per_move;

            auto hypothetical = board.make_hypothetical();
            hypothetical.submit_move(move);
            score = evaluate_board(hypothetical,
                                   clock,
                                   board.whose_turn(),
                                   positions_per_move,
                                   genome.evaluate(board, board.whose_turn()));
        }
        catch(const Checkmate_Exception&)
        {
            return move;
        }
        catch(const Game_Ending_Exception&) // stalemate
        {
            score = std::numeric_limits<double>::lowest();
        }

        if(score > best_score)
        {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}

double Genetic_AI::evaluate_board(const Board& board,
                                  const Clock& clock,
                                  Color perspective,
                                  int positions_to_examine,
                                  double original_board_score) const
{
    auto board_score = genome.evaluate(board, perspective);
    --positions_to_examine; // subtract one for examining this node of the game tree

    if(positions_to_examine > 0
       && clock.time_left(clock.running_for()) > std::max(genome.time_required(), 0.0))
    {
        try
        {
            auto next_board = board.make_hypothetical();

            // use remaining position examinations to find opponent's next move
            Complete_Move next_move = choose_move(next_board, clock, positions_to_examine);
            next_board.submit_move(next_move);

            // use same number of position examinations opponent's move (why it was chosen)
            return evaluate_board(next_board,
                                  clock,
                                  perspective,
                                  std::ceil(double(positions_to_examine)/board.all_legal_moves().size()),
                                  original_board_score);
        }
        catch(const Game_Ending_Exception& gee)
        {
            if(gee.winner() == perspective)
            {
                return std::numeric_limits<double>::max();
            }
            else if(gee.winner() == opposite(perspective))
            {
                return std::numeric_limits<double>::lowest();
            }
            else
            {
                return std::nexttoward(std::numeric_limits<double>::lowest(), 0);
            }
        }
    }

    return board_score;
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

bool Genetic_AI::operator==(const Genetic_AI& other) const
{
    return this == &other;
}

bool Genetic_AI::operator!=(const Genetic_AI& other) const
{
    return ! (*this == other);
}
