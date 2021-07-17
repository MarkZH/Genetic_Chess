#ifndef ITERATIVE_DEEPENING_AI_H
#define ITERATIVE_DEEPENING_AI_H

#include <string>

#include "Minimax_AI.h"

class Board;
class Clock;
class Move;

//! \brief An AI that chooses moves using the iterative deepening version of Minimax.
class Iterative_Deepening_AI : public Minimax_AI
{
    public:
        using Minimax_AI::Minimax_AI;

        std::string name() const noexcept override;

        const Move& choose_move(const Board& board, const Clock& clock) const noexcept override;
};

#endif // ITERATIVE_DEEPENING_AI_H
