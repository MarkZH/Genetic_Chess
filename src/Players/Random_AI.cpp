#include "Players/Random_AI.h"

#include <vector>
#include <string>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Utility/Random.h"

class Clock;

const Move& Random_AI::choose_move(const Board& board, const Clock&) const noexcept
{
    return *Random::random_element(board.legal_moves());
}

std::string Random_AI::name() const noexcept
{
    return "Random AI";
}

std::string Random_AI::author() const noexcept
{
    return "Mark Harrison";
}
