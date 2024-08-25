#include "Move_Decision.h"

Move_Decision::Move_Decision(const Move& move, const bool resigned, const bool checkmate) noexcept
    : chosen_move(move), player_resigned(resigned), game_ended_in_checkmate(checkmate)
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

bool Move_Decision::resigned_due_to_checkmate() const noexcept
{
    return game_ended_in_checkmate && resigned();
}
