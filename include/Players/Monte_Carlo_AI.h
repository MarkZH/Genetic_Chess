#ifndef MONTE_CARLO_AI_H
#define MONTE_CARLO_AI_H

#include <string>

#include "Random_AI.h"

class Board;
class Clock;
class Move;

class Monte_Carlo_AI : public Random_AI
{
    public:
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        std::string name() const override;

    private:
        void print_cecp_thinking(double time_so_far,
                                 double result,
                                 int move_count,
                                 int game_count,
                                 const Move* move,
                                 const Board& board) const;
};

#endif // MONTE_CARLO_AI_H
