#include "Players/Random_AI.h"

#include <vector>
#include <string>

#include "Game/Board.h"
#include "Moves/Move.h"
#include "Utility/Random.h"

class Clock;

//! Picks a random legal move.

//! \param board The current state of the board.
//! \param clock The game clock.
const Move& Random_AI::choose_move(const Board& board, const Clock&) const
{
    return *choose_random_move(board);
}

//! Implements the random move choice.

//! \param board The current board. The move is a random entry from board.legal_moves().
const Move* Random_AI::choose_random_move(const Board& board) const
{
    return Random::random_element(board.legal_moves());
}

std::string Random_AI::name() const
{
    return "Random AI";
}
