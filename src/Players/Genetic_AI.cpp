#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <cmath>

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
    const auto& legal_moves = board.all_legal_moves();
    if(legal_moves.size() == 1)
    {
        return legal_moves.front(); // If there's only one legal move, take it.
    }

    auto positions_to_examine = genome.positions_to_examine(board, clock);

    Game_Tree_Node_Result alpha_start = {Complete_Move(),
                                         -Math::infinity,
                                         board.whose_turn(),
                                         0,
                                         ""};
    Game_Tree_Node_Result beta_start = {Complete_Move(),
                                        Math::infinity,
                                        board.whose_turn(),
                                        0,
                                        ""};

    auto result = search_game_tree(board,
                                   positions_to_examine,
                                   clock,
                                   0,
                                   alpha_start,
                                   beta_start);
    if(result.depth > 0)
    {
        board.add_commentary_to_next_move(result.commentary);
    }
    return result.move;
}

bool better_than(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b, Color perspective)
{
    auto scoreA = a.corrected_score(perspective);
    auto scoreB = b.corrected_score(perspective);

    if(scoreA > scoreB)
    {
        return true;
    }

    if(scoreA < scoreB)
    {
        return false;
    }

    // scoreA == scoreB

    // Shorter path to winning is better
    if(scoreA == Math::win_score)
    {
        return a.depth < b.depth;
    }

    // Longer path to losing is better
    if(scoreA == Math::lose_score)
    {
        return a.depth > b.depth;
    }

    return false;
}

bool operator==(const Game_Tree_Node_Result& a, const Game_Tree_Node_Result& b)
{
    auto scoreA = a.corrected_score(WHITE);
    auto scoreB = b.corrected_score(WHITE);

    if(scoreA != scoreB)
    {
        return false;
    }

    // scoreA == scoreB

    if(std::abs(scoreA) == Math::win_score)
    {
        return a.depth == b.depth;
    }

    return true;
}

Game_Tree_Node_Result Genetic_AI::search_game_tree(const Board& board,
                                                   int& positions_to_examine,
                                                   const Clock& clock,
                                                   const int depth,
                                                   Game_Tree_Node_Result alpha,
                                                   Game_Tree_Node_Result beta) const
{
    // Every call to search_game_tree() after the first
    // costs a position_to_examine.
    if(depth > 0 && positions_to_examine > 0)
    {
        --positions_to_examine;
    }

    auto perspective = board.whose_turn();

    Game_Tree_Node_Result best_result = {board.all_legal_moves().front(),
                                         -Math::infinity,
                                         perspective,
                                         depth,
                                         ""};
    size_t moves_examined = 0;

    for(const auto& move : board.all_legal_moves())
    {
        if(clock.time_left(clock.running_for()) < 0.0)
        {
            return best_result;
        }

        auto next_board = board;

        try
        {
            next_board.submit_move(move);
        }
        catch(const Checkmate_Exception&)
        {
            // Mate in one (try to pick the shortest path to checkmate)
            auto score = genome.evaluate(next_board, perspective);
            auto comment = next_board.get_game_record().back();

            return {move,
                    score,
                    perspective,
                    depth,
                    comment};
        }
        catch(const Game_Ending_Exception&)
        {
            // Draws get scored like any other board position
        }

        Game_Tree_Node_Result result;

        if(next_board.game_has_ended()
           || ( next_board.all_legal_moves().size() > 1
                && (positions_to_examine <= 0
                    || ! genome.good_enough_to_examine(board, next_board, perspective))))
        {
            // Record immediate result without looking ahead further
            auto score = genome.evaluate(next_board, perspective);
            result = {move,
                      score,
                      perspective,
                      depth,
                      next_board.get_game_record().back()};
        }
        else
        {
            int moves_left = board.all_legal_moves().size() - moves_examined;
            int positions_for_this_move = positions_to_examine/moves_left;

            if(positions_for_this_move > 0 || next_board.all_legal_moves().size() == 1)
            {
                positions_to_examine -= positions_for_this_move;

                result = search_game_tree(next_board,
                                          positions_for_this_move,
                                          clock,
                                          depth + 1,
                                          beta,
                                          alpha);
                // Update last result with this game tree node's data
                result.move = move;
                result.commentary = next_board.get_game_record().back()
                                            + " "
                                            + result.commentary;

                positions_to_examine += positions_for_this_move;
            }
            else
            {
                result = {move,
                          genome.evaluate(next_board, perspective),
                          perspective,
                          depth,
                          next_board.get_game_record().back()};
            }
        }

        if(better_than(result, best_result, perspective))
        {
            best_result = result;
            if(better_than(result, alpha, perspective))
            {
                alpha = result;
                if(better_than(alpha, beta, perspective) || alpha == beta)
                {
                    break;
                }
            }
        }

        ++moves_examined;
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
