#ifndef MONTE_CARLO_SEARCH_TREE_H
#define MONTE_CARLO_SEARCH_TREE_H

#include <vector>
#include <memory>
#include <utility>

class Move;
class Board;

//! Storage for the sampled game tree used by the Monte_Carlo_AI.
//
//! This is a recursive class in that it contains pointers to
//! other instances of Monte_Carlo_Tree_Search to represent the
//! known game tree. Each instance keeps track of how many game
//! move lists pass through it and how many times that games through
//! that instances led to victory, defeat, or a draw.
class Monte_Carlo_Search_Tree
{
    using Move_Iterator = std::vector<const Move*>::const_iterator;

    public:
        //! Add a single random game to the game tree.
        //
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
        void add_search(Move_Iterator begin,
                        Move_Iterator end,
                        int score);
        
        //! Cut off branches representing moves not taken.
        //
        //! In order to save on memory, the branches of the game tree that were
        //! not realized in the actual game are pruned away, often leaving a tree
        //! around 1/20th the original size.
        //! \param move The last move made in the current game.
        void reroot(const Move* move);
        
        //! Find the most probable winning move in the branches
        //
        //! \returns A pair consisting of the most probable winning move (defined as
        //!          the maximum of (score)/(visists)) and the average score of
        //!          that move.
        //!
        //! The score of a move is the average score of the endgame results of all the
        //! random games that passed through this branch of the game tree (see add_search()).
        //! The idea is that the more often random games lead to winning outcomes when they pass
        //! through a node, the more likely that node represents a good move.
        std::pair<const Move*, double> best_result() const;
        
        //! Calculate the current value of a move.
        //
        //! \param move The move being queried.
        //! The value of a move is the average score (see add_search()) of
        //! all random games so far that started with the queried move.
        double current_score(const Move* move) const;

        //! View a portion of the game tree.
        //
        //! \param move Pick which branch to look at based on the next move.
        //! \returns A branch of the game tree that corresponds to the queried move.
        //!          Returns nullptr if no such branch exists.
        const Monte_Carlo_Search_Tree* subtree(const Move* move) const;
        
        //! Pick the next move to explore while searching the game tree.
        //
        //! \param board The current state of the board being searched.
        //! \returns If there are unexplored moves, return a random choice among those,
        //!          otherwise, return a move that is a compromise between moves
        //!          with high value outcomes and moves with little exploration.
        //!
        //! See article at (https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Exploration_and_exploitation).
        const Move* next_move(const Board& board) const;

    private:
        std::vector<const Move*> moves;
        std::vector<int> results;
        std::vector<int> visits;
        std::vector<std::unique_ptr<Monte_Carlo_Search_Tree>> branches;

        size_t index_of(const Move* move) const;
};

#endif // MONTE_CARLO_SEARCH_TREE_H
