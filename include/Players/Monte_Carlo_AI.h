#ifndef MONTE_CARLO_AI_H
#define MONTE_CARLO_AI_H

#include <string>
#include <thread>

#include "Random_AI.h"

#include "Monte_Carlo_Search_Tree.h"
#include "Game/Board.h"

class Clock;
class Move;

//! This player picks a move by randomly sampling the game tree.
//
//! This AI plays multiple random games to completion and then picks
//! the move that most often leads to victory.
//!
//! Monte Carlo Tree Search: https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
class Monte_Carlo_AI : public Random_AI
{
    public:
        ~Monte_Carlo_AI();

        const Move& choose_move(const Board& board, const Clock& clock) const override;
        void ponder(const Board& board, const Clock& clock) const override;

        std::string name() const override;

    private:
        mutable Monte_Carlo_Search_Tree search_tree;
        mutable Board ponder_board;
        mutable std::thread ponder_thread;

        const Move& pick_move(const Board& board, const Clock& clock, bool pondering) const;
        void stop_pondering() const;
        void print_cecp_thinking(double time_so_far,
                                 double search_time,
                                 double result,
                                 int move_count,
                                 int game_count,
                                 const Move* move,
                                 const Board& board) const;
};

#endif // MONTE_CARLO_AI_H
