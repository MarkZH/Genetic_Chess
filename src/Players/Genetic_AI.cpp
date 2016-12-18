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
    const auto& legal_moves = board.all_legal_moves();
    if(legal_moves.size() == 1)
    {
        return legal_moves.front(); // If there's only one legal move, take it.
    }

    auto positions_to_examine = genome.positions_to_examine(board, clock);
    std::string look_ahead_comment = "[" + std::to_string(positions_to_examine) + ",";
    auto time_at_start = clock.time_left(board.whose_turn());
    auto result = search_game_tree(board,
                                   positions_to_examine,
                                   clock,
                                   0);
    look_ahead_comment += std::to_string(positions_to_examine) + "] ";
    auto time_used = time_at_start - clock.time_left(board.whose_turn());
    board.add_commentary_to_next_move(look_ahead_comment
                                      + " "
                                      + std::to_string(time_used)
                                      + " "
                                      + result.commentary);
    return result.move;
}

Game_Tree_Node_Result Genetic_AI::search_game_tree(const Board& board,
                                                   int& positions_to_examine,
                                                   const Clock& clock,
                                                   const int depth) const
{
    // Every call to search_game_tree() after the first
    // costs a position_to_examine.
    if(depth > 0 && positions_to_examine > 0)
    {
        --positions_to_examine;
    }

    auto perspective = board.whose_turn();

    std::vector<Game_Tree_Node_Result> results;

    // Moves worth examining further
    std::vector<std::tuple<Complete_Move, Board, double>> further_examine; // move, resulting board, score

    // Find moves worth examining
    for(const auto& move : board.all_legal_moves())
    {
        if(clock.time_left(clock.running_for()) < 0.0)
        {
            return {board.all_legal_moves().front(),
                    0.0,
                    perspective,
                    depth,
                    "Out of time"};
        }

        auto next_board = board;

        try
        {
            next_board.submit_move(move);

            if(next_board.all_legal_moves().size() > 1 &&
               (positions_to_examine <= 0
               || ! genome.good_enough_to_examine(board, next_board, perspective)))
            {
                // Record immediate result without looking ahead further
                results.push_back({move,
                                   genome.evaluate(next_board, perspective),
                                   perspective,
                                   depth,
                                   next_board.get_game_record().back()});
            }
            else
            {
                further_examine.push_back({move, next_board, genome.evaluate(next_board, perspective)});
            }
        }
        catch(const Checkmate_Exception&)
        {
            // Mate in one (try to pick the shortest path to checkmate)
            auto score = genome.evaluate(next_board, perspective);
            auto comment = next_board.get_game_record().back();
            if(depth == 0)
            {
                comment += " (" + std::to_string(score) + ")";
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
            results.push_back({move,
                               genome.evaluate(next_board, perspective),
                               perspective,
                               depth,
                               next_board.get_game_record().back()});
        }
    }


    // Sort moves by score so higher scores get more moves for examination
    std::sort(further_examine.begin(),
              further_examine.end(),
              [](const auto& x, const auto& y)
              {
                  return std::get<double>(x) < std::get<double>(y);
              });

    // Look ahead through moves found above
    for(size_t i = 0; i < further_examine.size(); ++i)
    {
        int moves_left = further_examine.size() - i;
        int positions_for_this_move = positions_to_examine/moves_left;

        if(positions_for_this_move > 0 || std::get<Board>(further_examine[i]).all_legal_moves().size() == 1)
        {
            positions_to_examine -= positions_for_this_move;

            results.push_back(search_game_tree(std::get<Board>(further_examine[i]),
                                               positions_for_this_move,
                                               clock,
                                               depth + 1));
            // Update last result with this node's data
            results.back().move = std::get<Complete_Move>(further_examine[i]);
            results.back().commentary = std::get<Board>(further_examine[i]).get_game_record().back()
                                        + " "
                                        + results.back().commentary;

            positions_to_examine += positions_for_this_move;
        }
        else
        {
            results.push_back({std::get<Complete_Move>(further_examine[i]),
                               std::get<double>(further_examine[i]),
                               perspective,
                               depth,
                               std::get<Board>(further_examine[i]).get_game_record().back()});
        }
    }

    // Consider all results and return best
    auto best_score = -Math::infinity;
    auto best_move = board.all_legal_moves().front();
    auto best_depth = 0;
    std::string comments_on_best_move;
    std::string comments_on_all_moves; // only use when depth == 0

    for(const auto& result : results)
    {
        auto score = result.score;
        if(result.perspective != perspective)
        {
            score = -score;
        }

        // Prefer ...
        if(score > best_score // ... better score
           || (score ==  Math::infinity && result.depth < best_depth) // shortest path to victory
           || (score == -Math::infinity && result.depth > best_depth)) // longest path to defeat
        {
            best_score = score;
            best_move = result.move;
            best_depth = result.depth;
            comments_on_best_move = result.commentary;
        }

        if(depth == 0)
        {
            // build comment on all current move possibilities
            comments_on_all_moves += " " + result.commentary + " (" + std::to_string(score) + ")";
        }
    }

    return {best_move,
            best_score,
            perspective,
            best_depth,
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
