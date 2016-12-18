#include "Players/Random_AI.h"

#include <vector>

#include "Game/Board.h"
#include "Moves/Move.h"
class Clock;

#include "Exceptions/Illegal_Move_Exception.h"

#include "Utility.h"

const Complete_Move Random_AI::choose_move(const Board& board, const Clock& /*clock*/) const
{
    const auto& moves = board.all_legal_moves();
    return moves[Random::random_integer(0, moves.size() - 1)];
}

Random_AI::~Random_AI()
{
}

std::string Random_AI::name() const
{
    return "Random AI";
}
