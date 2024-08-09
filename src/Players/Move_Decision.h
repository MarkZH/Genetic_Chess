#ifndef MOVE_DECISION_H
#define MOVE_DECISION_H

#include "Game/Move.h"

class Move_Decision
{
    public:
        Move_Decision(const Move& move, bool resigned) noexcept;
        const Move& move() const noexcept;
        bool resigned() const noexcept;

    private:
        const Move& chosen_move;
        bool player_resigned;
};

#endif // !MOVE_DECISION_H

