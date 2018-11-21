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
    if(index >= results.size())
    {
        moves.push_back(*begin);
        results.push_back(0);
        branches.push_back(std::make_unique<Monte_Carlo_Search_Tree>());
        visits.push_back(0);

        assert(moves.size() == index + 1);
        assert(results.size() == index + 1);
        assert(branches.size() == index + 1);
        assert(visits.size() == index + 1);
    }
    results[index] += score;
    branches[index]->add_search(std::next(begin), end, score);
    ++visits[index];
}

void Monte_Carlo_Search_Tree::reroot(const Move* move)
{
    auto index = index_of(move);
    if(index >= moves.size())
    {
        moves.clear();
        results.clear();
        branches.clear();
        visits.clear();

        return;
    }

    results = branches[index]->results;
    visits = branches[index]->visits;
    branches = std::move(branches[index]->branches);
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
