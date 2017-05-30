#include "Players/Genetic_AI.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "Moves/Complete_Move.h"
#include "Game/Board.h"
#include "Game/Clock.h"

#include "Exceptions/Checkmate_Exception.h"
#include "Exceptions/Game_Ending_Exception.h"

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

const Complete_Move Genetic_AI::choose_move(const Board& board, const Clock& clock) const
{
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

        return legal_moves.front(); // If there's only one legal move, take it.
    }

    auto time_to_use = genome.time_to_examine(board, clock);

    // alpha = highest score found that opponent will allow
    Game_Tree_Node_Result alpha_start = {Complete_Move(),
                                         Math::lose_score,
                                         board.whose_turn(),
                                         0,
                                         {}};

    // beta = score that will cause opponent to choose a different prior move
    Game_Tree_Node_Result beta_start = {Complete_Move(),
                                        Math::win_score,
                                        board.whose_turn(),
                                        0,
                                        {}};

    auto result = search_game_tree(board,
                                   time_to_use,
                                   clock,
                                   0,
                                   alpha_start,
                                   beta_start);

    if(result.depth > 0)
    {
        principal_variation = result.commentary;
    }
    else
    {
        principal_variation.clear();
    }
    commentary.push_back(principal_variation);

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
                                                   const double time_to_examine,
                                                   const Clock& clock,
                                                   const size_t depth,
                                                   Game_Tree_Node_Result alpha,
                                                   Game_Tree_Node_Result beta) const
{
    auto time_start = clock.time_left(clock.running_for());
    auto all_legal_moves = board.legal_moves();

    // The first item in the principal variation is the last move that
    // this player made. Since then, the opponent has also made a move,
    // so the first item in the principal variation corresponds to the
    // game record item at game_record.size() - 2. The depth of the game
    // tree search increments both the game_record index and the principal
    // variation index in step.
    bool still_on_principal_variation = false;
    if(principal_variation.size() > depth + 2)
    {
        still_on_principal_variation = std::equal(board.get_game_record().end() - 2 - depth,
                                                  board.get_game_record().end(),
                                                  principal_variation.begin());

        if(still_on_principal_variation)
        {
            auto next_principal_variation_move = board.get_complete_move(principal_variation[depth + 2]);
            auto move_iter = std::find(all_legal_moves.begin(),
                                       all_legal_moves.end(),
                                       next_principal_variation_move);

            // Make sure that the principal variation is actually a legal move.
            // This is purely for debugging as special circumstances (i.e., once
            // per several thousand games) cause the principal variation to be
            // invalidated without it being cleared.
            if(move_iter == all_legal_moves.end())
            {
                board.ascii_draw(WHITE);
                board.print_game_record(nullptr, nullptr, "", "Principal Variation error");
                auto board_copy = board;
                for(const auto& item : principal_variation)
                {
                    auto move = board_copy.get_complete_move(item);
                    std::cout << move.game_record_item(board_copy) << " ";
                    board_copy.submit_move(move);
                }
                std::cout << '\n' << "Depth: " << depth << '\n'
                          << "Next move in variation: "
                          << next_principal_variation_move.game_record_item(board)
                          << std::endl;
                throw std::runtime_error("ERROR: bad variation code");
            }

            // Put principal variation move at start of list to allow
            // the most pruning later.
            std::iter_swap(all_legal_moves.begin(), move_iter);
        }
    }

    auto perspective = board.whose_turn();
    int moves_examined = 0;
    const auto current_legal_moves_count = all_legal_moves.size();

    Game_Tree_Node_Result best_result = {board.legal_moves().front(),
                                         Math::lose_score,
                                         perspective,
                                         depth,
                                         {}};

    for(const auto& move : all_legal_moves)
    {
        auto next_board = board;

        try
        {
            next_board.submit_move(move);
        }
        catch(const Checkmate_Exception&)
        {
            // Mate in one (try to pick the shortest path to checkmate)
            return {move,
                    genome.evaluate(next_board, perspective),
                    perspective,
                    depth,
                    {next_board.get_game_record().end() - (depth + 1),
                     next_board.get_game_record().end()}};
        }
        catch(const Game_Ending_Exception&)
        {
            // Draws get scored like any other board position
        }

        if(alpha.corrected_score(perspective) == Math::win_score
           && alpha.depth <= depth + 2)
        {
            // This move will take a longer path to victory
            // than one already found. Use "depth + 2" since,
            // if this move isn't winning (and it isn't, since
            // we're here), then the earliest move that can
            // win is the next one, which is two away (after
            // opponent's move).
            continue;
        }

        int moves_left = current_legal_moves_count - moves_examined;
        double time_left = time_to_examine - (time_start - clock.time_left(clock.running_for()));
        double time_allotted_for_this_move = time_left/moves_left;

        bool recurse;
        if(next_board.game_has_ended())
        {
            recurse = false;
        }
        else if(next_board.legal_moves().size() == 1)
        {
            recurse = true;
        }
        else if(still_on_principal_variation)
        {
            recurse = true;
        }
        else
        {
            recurse = genome.enough_time_to_recurse(time_allotted_for_this_move, next_board);
        }

        Game_Tree_Node_Result result;
        if(recurse)
        {
            result = search_game_tree(next_board,
                                      time_allotted_for_this_move,
                                      clock,
                                      depth + 1,
                                      beta,
                                      alpha);

            // Update last result with this game tree node's data
            result.move = move;
        }
        else
        {
            // Record immediate result without looking ahead further
            result = {move,
                      genome.evaluate(next_board, perspective),
                      perspective,
                      depth,
                      {next_board.get_game_record().end() - (depth + 1),
                       next_board.get_game_record().end()}};
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
            }
        }

        ++moves_examined;
        still_on_principal_variation = false; // only the first move is part of the principal variation

        if(clock.time_left(clock.running_for()) < 0)
        {
            break;
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
        result = commentary.at(move_number).front();
        for(size_t i = 1; i < commentary.at(move_number).size(); ++i)
        {
            result += " " + commentary.at(move_number).at(i);
        }
    }

    return result;
}
