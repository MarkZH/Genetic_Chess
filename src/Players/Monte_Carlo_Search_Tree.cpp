#include "Players/Monte_Carlo_Search_Tree.h"

#include <vector>
#include <iterator>
#include <numeric>
#include <cassert>
#include <limits>
#include <cmath>
#include <utility>

#include "Game/Board.h"
#include "Utility/Random.h"

class Move;

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
    return size_t(std::distance(moves.begin(), iter));
}

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
        auto explore_choice = Random::random_integer<size_t>(0, unexplored_moves - 1);
        size_t explore_index = 0;
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
