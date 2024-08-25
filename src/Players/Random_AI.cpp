#include "Players/Random_AI.h"

#include <vector>
#include <string>

#include "Game/Board.h"
#include "Game/Move.h"
#include "Players/Move_Decision.h"
#include "Utility/Random.h"

class Clock;

Move_Decision Random_AI::choose_move(const Board& board, const Clock&) const noexcept
{
    return {*Random::random_element(board.legal_moves()), Random::success_probability(1, 1000), false};
}

std::string Random_AI::name() const noexcept
{
    return "Random AI";
}

std::string Random_AI::author() const noexcept
{
    return "Mark Harrison";
}
