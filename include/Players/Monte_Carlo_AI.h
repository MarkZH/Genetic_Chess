#ifndef MONTE_CARLO_AI_H
#define MONTE_CARLO_AI_H

#include <string>

#include "Random_AI.h"

#include "Monte_Carlo_Search_Tree.h"

class Board;
class Clock;
class Move;

//! This player picks a move by randomly sampling the game tree.

//! This AI plays multiple random games to completion and then picks 
//! the move that most often leads to victory.
//!
//! Monte Carlo Tree Search: https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
class Monte_Carlo_AI : public Random_AI
{
    public:
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        std::string name() const override;

    private:
        mutable Monte_Carlo_Search_Tree search_tree;

        void print_cecp_thinking(double time_so_far,
                                 double search_time,
                                 double result,
                                 int move_count,
                                 int game_count,
                                 const Move* move,
                                 const Board& board) const;
};

#endif // MONTE_CARLO_AI_H
