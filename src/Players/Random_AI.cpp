#include "Players/Random_AI.h"

#include <vector>

#include "Players/Player.h"
#include "Game/Board.h"
#include "Moves/Move.h"
#include "Utility.h"

class Clock;

const Move& Random_AI::choose_move(const Board& board, const Clock& /*clock*/) const
{
    const auto& moves = board.legal_moves();
    return *moves[Random::random_integer(0, moves.size() - 1)];
}

std::string Random_AI::name() const
{
    return "Random AI";
}
