#ifndef MOVE_DECISION_H
#define MOVE_DECISION_H

#include "Game/Move.h"

class Move_Decision
{
    public:
        Move_Decision(const Move& move, bool resigned, bool checkmate) noexcept;
        const Move& move() const noexcept;
        bool resigned() const noexcept;
        bool resigned_due_to_checkmate() const noexcept;

    private:
        const Move& chosen_move;
        bool player_resigned;
        bool game_ended_in_checkmate;
};

#endif // !MOVE_DECISION_H

