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
        //! Make sure the pondering thread is stopped.
        ~Monte_Carlo_AI();

        //! Builds up a sample of the game tree with random complete games to estimate the best move.
        //
        //! As the AI plays more random complete games, it fills out the game tree. After some
        //! amount of time (estimated to give equal time to every move in the game) it picks the
        //! move that most often leads to victory. During the next move, the branches of the
        //! game tree not taken are cutoff to keep memory use under control.
        //! \param board The current state of the game board.
        //! \param clock The game clock.
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        void ponder(const Board& board, const Clock& clock, bool thinking_allowed) const override;

        //! The name is the same as the algorithm.
        //
        //! \returns "Monte Carlo AI"
        std::string name() const override;

    private:
        mutable Monte_Carlo_Search_Tree search_tree;
        mutable Board ponder_board;
        mutable std::thread ponder_thread;
        mutable bool pondered = false;

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
