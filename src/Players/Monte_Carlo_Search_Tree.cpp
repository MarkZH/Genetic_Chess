#include "Players/Monte_Carlo_Search_Tree.h"

#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <limits>

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

void Monte_Carlo_Search_Tree::reroot(Move_Iterator begin, Move_Iterator end)
{
    auto next_branch = this;

    for(auto current = begin; current != end; current = std::next(current))
    {
        auto index = next_branch->index_of(*current);
        if(index < next_branch->branches.size())
        {
            next_branch = next_branch->branches[index].get();
        }
        else
        {
            next_branch = nullptr;
        }

        if( ! next_branch)
        {
            moves.clear();
            results.clear();
            visits.clear();
            branches.clear();
            return;
        }
    }

    if(next_branch != this)
    {
        moves = next_branch->moves;
        results = next_branch->results;
        visits = next_branch->visits;
        branches = std::move(next_branch->branches);
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

    return {moves[best_index], best_score};
}

size_t Monte_Carlo_Search_Tree::index_of(const Move* move) const
{
    auto iter = std::find(moves.begin(), moves.end(), move);
    return std::distance(moves.begin(), iter);
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
