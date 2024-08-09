#include "Move_Decision.h"

Move_Decision::Move_Decision(const Move& move, bool resigned) noexcept : chosen_move(move), player_resigned(resigned)
{
}

const Move& Move_Decision::move() const noexcept
{
    return chosen_move;
}

bool Move_Decision::resigned() const noexcept
{
    return player_resigned;
}
