#ifndef MONTE_CARLO_SEARCH_TREE_H
#define MONTE_CARLO_SEARCH_TREE_H

#include <vector>
#include <memory>
#include <utility>

class Move;

class Monte_Carlo_Search_Tree
{
    using Move_Iterator = std::vector<const Move*>::const_iterator;

    public:
        void add_search(Move_Iterator begin,
                        Move_Iterator end,
                        int score);
        void reroot(const Move* move);
        std::pair<const Move*, double> best_result() const;
        double current_score(const Move* move) const;

    private:
        std::vector<const Move*> moves;
        std::vector<int> results;
        std::vector<int> visits;
        std::vector<std::unique_ptr<Monte_Carlo_Search_Tree>> branches;

        size_t index_of(const Move* move) const;
};

#endif // MONTE_CARLO_SEARCH_TREE_H
