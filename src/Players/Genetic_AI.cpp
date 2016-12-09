#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"

#include "Exceptions/Checkmate_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"
#include "Exceptions/End_Of_File_Exception.h"

#include "Utility.h"

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

    auto result = search_game_tree(board,
                                   genome.positions_to_examine(board, clock),
                                   clock,
                                   0);
    board.add_commentary_to_next_move(result.commentary);
    return result.move;
}

Game_Tree_Node_Result Genetic_AI::search_game_tree(const Board& board,
                                                   double positions_to_examine,
                                                   const Clock& clock,
                                                   const int depth) const
{
    auto perspective = board.whose_turn();
    auto legal_moves = board.all_legal_moves();
    auto positions_per_move = positions_to_examine/legal_moves.size();

    auto best_score = -Math::infinity;
    auto best_move = legal_moves.front();
    auto best_depth = 0;
    std::string comments_on_best_move;

    std::string comments_on_all_moves; // only use when depth == 0

    for(const auto& move : legal_moves)
    {
        if(clock.time_left(clock.running_for()) < 0.0)
        {
            break;
        }

        auto next_board = board;
        double score;
        std::string comments_on_this_move;
        auto local_depth = depth;
        try
        {
            next_board.submit_move(move);
            if(positions_per_move < 1)
            {
                score = genome.evaluate(next_board, perspective);
                comments_on_this_move = next_board.get_game_record().back() + " (" + std::to_string(score) + ")";
            }
            else
            {
                auto result = search_game_tree(next_board,
                                               positions_per_move,
                                               clock,
                                               depth + 1);
                score = result.score;
                if(result.perspective != perspective)
                {
                    score = -score;
                }
                local_depth = result.depth;
                comments_on_this_move = next_board.get_game_record().back() + " " + result.commentary;
            }
        }
        catch(const Checkmate_Exception&)
        {
            // Mate in one (try to pick the shortest path to checkmate)
            score = genome.evaluate(next_board, perspective);
            std::string comment = next_board.get_game_record().back() + " (" + std::to_string(score) + ")";
            if(depth == 0)
            {
                comment = comments_on_all_moves + " " + comment;
            }
            return {move,
                    score,
                    perspective,
                    depth,
                    comment};
        }
        catch(const Game_Ending_Exception&)
        {
            // Draw
            score = genome.evaluate(next_board, perspective);
            comments_on_this_move =  next_board.get_game_record().back() + " (" + std::to_string(score) + ")";
        }

        // Prefer ...
        if(score > best_score // ... better score
           || (score ==  Math::infinity && depth < best_depth) // shortest path to victory
           || (score == -Math::infinity && depth > best_depth)) // longest path to defeat
        {
            best_score = score;
            best_move = move;
            best_depth = local_depth;
            comments_on_best_move = comments_on_this_move;
        }

        if(depth == 0)
        {
            comments_on_all_moves += " " + comments_on_this_move;
            if(comments_on_all_moves.size() > 1e6)
            {
                throw std::runtime_error("Move commentary too large: " + std::to_string(comments_on_all_moves.size()));
            }
        }
    }

    return {best_move,
            best_score,
            perspective,
            depth,
            (depth == 0 ? comments_on_all_moves : comments_on_best_move)};
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
    return "Genetic AI " + std::to_string(get_id());
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
