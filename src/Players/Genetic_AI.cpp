#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cassert>

#include "Players/Player.h"
#include "Moves/Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Players/Thinking.h"
#include "Players/Game_Tree_Node_Result.h"

#include "Utility.h"

int Genetic_AI::next_id = 0;

Genetic_AI::Genetic_AI() :
    genome(),
    id(next_id++)
{
    calibrate_thinking_speed();
}

// Sexual reproduction
Genetic_AI::Genetic_AI(const Genetic_AI& A, const Genetic_AI& B) :
    genome(A.genome, B.genome),
    id(next_id++)
{
    calibrate_thinking_speed();
}

Genetic_AI::Genetic_AI(const std::string& file_name)
{
    std::ifstream ifs(file_name);
    if( ! ifs)
    {
        throw std::runtime_error("Could not read: " + file_name);
    }

    read_from(ifs);

    calibrate_thinking_speed();
}

Genetic_AI::Genetic_AI(std::istream& is)
{
    read_from(is);

    calibrate_thinking_speed();
}

Genetic_AI::Genetic_AI(const std::string& file_name, int id_in) : id(id_in)
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
            genome.read_from(ifs);

            calibrate_thinking_speed();

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
        throw std::runtime_error("Incomplete Genetic_AI spec in file for ID " + std::to_string(id));
    }

    if(id >= next_id)
    {
        next_id = id + 1;
    }

    genome.read_from(is);
}

const Move& Genetic_AI::choose_move(const Board& board, const Clock& clock) const
{
    // Erase data from previous board when starting new game
    if(board.get_game_record().size() <= 1)
    {
        principal_variation.clear();
        commentary.clear();
    }

    nodes_searched = 0;
    clock_start_time = clock.time_left(clock.running_for());
    maximum_depth = 0;

    nodes_evaluated = 0;
    total_evaluation_time = 0.0;

    const auto& legal_moves = board.legal_moves();
    if(legal_moves.size() == 1)
    {
        if(principal_variation.size() > 2 && principal_variation[1] == board.get_game_record().back())
        {
            // search_game_tree() assumes the principal variation starts
            // with the previous move of this player. If a move was forced,
            // then the principal variation needs to be updated to start with
            // the next move of this side after checking that the immediately
            // preceding move was the expected one.
            principal_variation.erase(principal_variation.begin(),
                                      principal_variation.begin() + 2);
        }
        else
        {
            principal_variation.clear();
        }

        commentary.push_back(principal_variation);

        return *legal_moves.front(); // If there's only one legal move, take it.
    }

    auto time_to_use = genome.time_to_examine(board, clock);

    // alpha = highest score found that opponent will allow
    Game_Tree_Node_Result alpha_start = {Math::lose_score,
                                         board.whose_turn(),
                                         {nullptr}};

    // beta = score that will cause opponent to choose a different prior move
    Game_Tree_Node_Result beta_start = {Math::win_score,
                                        board.whose_turn(),
                                        {nullptr}};

    auto result = search_game_tree(board,
                                   time_to_use,
                                   clock,
                                   0,
                                   alpha_start,
                                   beta_start,
                                   ! principal_variation.empty());

    if(board.get_thinking_mode() == CECP)
    {
        output_thinking_cecp(result, clock, board.whose_turn());
        std::cout << std::flush;
    }

    if(result.depth() > 0)
    {
        commentary.push_back(result.variation);
    }
    else
    {
        commentary.push_back({});
    }

    if(result.depth() > 1)
    {
        principal_variation = result.variation;
    }
    else
    {
        principal_variation.clear();
    }

    evaluation_speed = nodes_evaluated/total_evaluation_time;

    return *result.variation.front();
}

Game_Tree_Node_Result Genetic_AI::search_game_tree(const Board& board,
                                                   const double time_to_examine,
                                                   const Clock& clock,
                                                   const size_t depth,
                                                   Game_Tree_Node_Result alpha,
                                                   const Game_Tree_Node_Result& beta,
                                                   bool still_on_principal_variation) const
{
    auto time_start = clock.time_left(clock.running_for());
    maximum_depth = std::max(maximum_depth, depth);
    auto all_legal_moves = board.legal_moves();

    // The first item in the principal variation is the last move that
    // this player made. Since then, the opponent has also made a move,
    // so the first item in the principal variation corresponds to the
    // game record item at game_record.size() - 2. The depth of the game
    // tree search increments both the game_record index and the principal
    // variation index in step.
    if(still_on_principal_variation && principal_variation.size() > depth + 2)
    {
        // Previous move matches prediction
        still_on_principal_variation =
            (board.get_game_record().back() == principal_variation[depth + 1]);

        if(still_on_principal_variation)
        {
            auto next_principal_variation_move = principal_variation[depth + 2];
            auto move_iter = std::find(all_legal_moves.begin(),
                                       all_legal_moves.end(),
                                       next_principal_variation_move);

            assert(move_iter != all_legal_moves.end());

            // Put principal variation move at start of list to allow
            // the most pruning later.
            std::iter_swap(all_legal_moves.begin(), move_iter);
        }
    }
    else
    {
        still_on_principal_variation = false;
    }

    auto perspective = board.whose_turn();
    auto moves_left = all_legal_moves.size();

    Game_Tree_Node_Result best_result = {Math::lose_score,
                                         perspective,
                                         {all_legal_moves.front()}};

    for(const auto& move : all_legal_moves)
    {
        auto evaluate_start_time = clock.time_left(clock.running_for());
        ++nodes_searched;

        auto next_board = board;

        auto move_result = next_board.submit_move(*move);
        if(move_result.get_winner() != NONE)
        {
            // This move results in checkmate, no other move can be better.
            return create_result(next_board, perspective, move_result, depth);
        }

        if(alpha.depth() <= depth + 2 && alpha.corrected_score(perspective) == Math::win_score)
        {
            // This move will take a longer path to victory
            // than one already found. Use "depth + 2" since,
            // if this move isn't winning (and it isn't, since
            // we're here), then the earliest move that can
            // win is the next one, which is two away (after
            // opponent's move).
            continue;
        }

        double time_left = time_to_examine - (time_start - clock.time_left(clock.running_for()));
        double time_allotted_for_this_move = time_left/moves_left;

        bool recurse;
        if(move_result.game_has_ended())
        {
            recurse = false;
        }
        else if(still_on_principal_variation)
        {
            recurse = true;
        }
        else
        {
            recurse = genome.enough_time_to_recurse(time_allotted_for_this_move, next_board, evaluation_speed);
        }

        Game_Tree_Node_Result result;
        if(recurse)
        {
            result = search_game_tree(next_board,
                                      time_allotted_for_this_move,
                                      clock,
                                      depth + 1,
                                      beta,
                                      alpha,
                                      still_on_principal_variation);
        }
        else
        {
            // Record immediate result without looking ahead further
            result = create_result(next_board, perspective, move_result, depth);
        }

        if(better_than(result, best_result, perspective))
        {
            best_result = result;

            if(better_than(best_result, alpha, perspective))
            {
                alpha = best_result;
                if(better_than(alpha, beta, perspective) || alpha == beta)
                {
                    break;
                }
                else if(board.get_thinking_mode() == CECP && ! recurse)
                {
                    output_thinking_cecp(alpha, clock, perspective);
                }
            }
        }

        --moves_left;
        still_on_principal_variation = false; // only the first move is part of the principal variation

        if(clock.time_left(clock.running_for()) < 0)
        {
            break;
        }

        if( ! recurse) // This move was scored by genome.evaluate().
        {
            ++nodes_evaluated;
            total_evaluation_time += evaluate_start_time - clock.time_left(clock.running_for());
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
    return "Genetic AI " + std::to_string(get_id());
}

std::string Genetic_AI::author() const
{
    return "Mark Harrison";
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

std::string Genetic_AI::get_commentary_for_move(size_t move_number) const
{
    std::string result;
    if(move_number < commentary.size() && ! commentary.at(move_number).empty())
    {
        result = commentary.at(move_number).front()->coordinate_move();
        for(size_t i = 1; i < commentary.at(move_number).size(); ++i)
        {
            result += " " + commentary.at(move_number).at(i)->coordinate_move();
        }
    }

    return result;
}

void Genetic_AI::output_thinking_cecp(const Game_Tree_Node_Result& thought,
                                      const Clock& clock,
                                      Color perspective) const
{
    auto score = thought.corrected_score(perspective);

    // Indicate "mate in N moves" where N == thought.depth
    if(score == Math::win_score)
    {
        score = 10000.0 - thought.depth();
    }
    else if(score == Math::lose_score)
    {
        score = -(10000.0 - thought.depth());
    }

    auto time_so_far = clock_start_time - clock.time_left(clock.running_for());
    std::cout << thought.depth() // ply
              << " "
              << int(score) // score in what should be centipawns
              << " "
              << int(time_so_far*100) // search time in centiseconds
              << " "
              << nodes_searched
              << " "
              << maximum_depth
              << " "
              << int(nodes_searched/time_so_far)
              << '\t';

    // Principal variation
    for(const auto& move : thought.variation)
    {
        std::cout << move->coordinate_move() << ' ';
    }

    std::cout << '\n';
}

void Genetic_AI::calibrate_thinking_speed()
{
    evaluation_speed = 100; // very conservative initial guess
    auto calibration_time = 1.0; // seconds
    Board board;
    Clock clock(calibration_time, 1, 0.0);
    clock.start();
    choose_move(board, clock);
    // choose_move() keeps track of the time it takes and the number of positions
    // it sees, so this practice move will update the positions_per_second to a
    // more reasonable value.
}

Game_Tree_Node_Result Genetic_AI::create_result(const Board& board,
                                                Color perspective,
                                                Game_Result move_result,
                                                size_t depth) const
{
    return {genome.evaluate(board, move_result, perspective),
            perspective,
            {board.get_game_record().end() - (depth + 1),
             board.get_game_record().end()}};
}
