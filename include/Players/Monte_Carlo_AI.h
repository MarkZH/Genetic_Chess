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
};

#endif // MONTE_CARLO_AI_H
