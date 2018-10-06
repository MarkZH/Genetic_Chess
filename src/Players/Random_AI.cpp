#include "Players/Random_AI.h"

#include <vector>
#include <string>

#include "Game/Board.h"
#include "Utility.h"

class Move;
class Clock;

const Move& Random_AI::choose_move(const Board& board, const Clock& /*clock*/) const
{
    const auto& moves = board.legal_moves();
    return *moves[Random::random_integer(0, int(moves.size()) - 1)];
}

std::string Random_AI::name() const
{
    return "Random AI";
}
