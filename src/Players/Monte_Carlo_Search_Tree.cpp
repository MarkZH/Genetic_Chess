#include "Players/Monte_Carlo_Search_Tree.h"

#include <vector>
#include <iterator>
#include <numeric>
#include <cassert>
#include <limits>
#include <cmath>

#include "Game/Board.h"
#include "Utility/Random.h"

class Move;

//! Add a single random game to the game tree.

//! \param begin An iterator (taken from a board.game_record()) to the
//!        first new move in a randomly played game.
//! \param end An iterator to the end of a board game record (i.e.,
//!        board.game_record().end().
//! \param score The value of the result of the randomly played game.
//!        Win = 1; Draw = 0; Loss = -1.
//!
//! The sequence of moves is fed into the tree such that each subsequent
//! move gets placed at a lower depth than the previous move. New branches
//! are created when a novel variation is added to the tree.
void Monte_Carlo_Search_Tree::add_search(Move_Iterator begin,
                                         Move_Iterator end,
                                         int score)
{
    if(begin == end)
    {
        return;
    }

    auto index = index_of(*begin);
    auto next = std::next(begin);
    if(index >= results.size())
    {
        moves.push_back(*begin);
        results.push_back(0);
        visits.push_back(0);
        if(next != end)
        {
            branches.push_back(std::make_unique<Monte_Carlo_Search_Tree>());
        }
        else
        {
            branches.push_back(nullptr);
        }

        assert(moves.size() == index + 1);
        assert(results.size() == index + 1);
        assert(visits.size() == index + 1);
        assert(branches.size() == index + 1);
    }
    results[index] += score;
    ++visits[index];
    if(branches[index])
    {
        branches[index]->add_search(next, end, score);
    }
}

//! Cut off branches representing moves not taken.

//! In order to save on memory, the branches of the game tree that were
//! not realized in the actual game are pruned away, often leaving a tree
//! around 1/20th the original size.
//! \param move The last move made in the current game.
void Monte_Carlo_Search_Tree::reroot(const Move* move)
{
    auto index = index_of(move);
    if(index < moves.size())
    {
        auto next_branch = branches[index].get();
        moves = next_branch->moves;
        results = next_branch->results;
        visits = next_branch->visits;
        branches = std::move(next_branch->branches);
    }
    else
    {
        moves.clear();
        results.clear();
        visits.clear();
        branches.clear();
    }
}

//! Find the most probable winning move in the branches

//! \returns A pair consisting of the most probable winning move (defined as
//!          the maximum of (score)/(visists)) and the average score of
//!          that move.
//!
//! The score of a move is the average score of the endgame results of all the
//! random games that passed through this branch of the game tree (see add_search()).
//! The idea is that the more often random games lead to winning outcomes when they pass
//! through a node, the more likely that node represents a good move.
std::pair<const Move*, double> Monte_Carlo_Search_Tree::best_result() const
{
    auto best_score = std::numeric_limits<double>::lowest();
    size_t best_index = 0;
    for(size_t index = 0; index < results.size(); ++index)
    {
        auto average_score = double(results[index])/visits[index];
        if(average_score > best_score)
        {
            best_score = average_score;
            best_index = index;
        }
    }

    if(best_index < moves.size())
    {
        return {moves[best_index], best_score};
    }
    else
    {
        return {nullptr, best_score};
    }
}

size_t Monte_Carlo_Search_Tree::index_of(const Move* move) const
{
    auto iter = std::find(moves.begin(), moves.end(), move);
    return std::distance(moves.begin(), iter);
}

//! Calculate the current value of a move.

//! \param move The move being queried.
//! The value of a move is the average score (see add_search()) of
//! all random games so far that started with the queried move.
double Monte_Carlo_Search_Tree::current_score(const Move* move) const
{
    auto index = index_of(move);
    if(index < results.size())
    {
        return double(results[index])/visits[index];
    }
    else
    {
        return 0.0;
    }
}

//! View a portion of the game tree.

//! \param move Pick which branch to look at based on the next move.
//! \returns A branch of the game tree that corresponds to the queried move.
//!          Returns nullptr if no such branch exists.
const Monte_Carlo_Search_Tree* Monte_Carlo_Search_Tree::subtree(const Move* move) const
{
    auto index = index_of(move);
    if(index < moves.size())
    {
        return branches[index].get();
    }
    else
    {
        return nullptr;
    }

}

//! Pick the next move to explore while searching the game tree.

//! \param board The current state of the board being searched.
//! \returns If there are unexplored moves, return a random choice among those,
//!          otherwise, return a move that is a compromise between moves
//!          with high value outcomes and moves with little exploration.
//!
//! See article at (https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Exploration_and_exploitation).
const Move* Monte_Carlo_Search_Tree::next_move(const Board& board) const
{
    const Move* chosen_move = board.legal_moves().front();
    auto unexplored_moves = board.legal_moves().size() - moves.size();
    if(unexplored_moves == 0)
    {
        auto best_score = std::numeric_limits<double>::lowest();
        auto N = std::accumulate(visits.begin(), visits.end(), 0);
        for(size_t i = 0; i < moves.size(); ++i)
        {
            static const auto c = std::sqrt(2);
            auto score = double(results[i])/visits[i] + c*std::sqrt(std::log(N)/visits[i]);
            if(score > best_score)
            {
                best_score = score;
                chosen_move = moves[i];
            }
        }
    }
    else
    {
        // Choose a random move that is not in this->moves(),
        auto explore_choice = Random::random_integer(0, int(unexplored_moves) - 1);
        auto explore_index = 0;
        for(auto move : board.legal_moves())
        {
            if(index_of(move) > moves.size()) // move not explored
            {
                if(explore_choice == explore_index)
                {
                    chosen_move = move;
                    break;
                }
                else
                {
                    ++explore_index;
                }
            }
        }
    }

    return chosen_move;
}
