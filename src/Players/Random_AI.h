#ifndef RANDOM_AI_H
#define RANDOM_AI_H

#include <string>

#include "Player.h"

class Board;
class Clock;
class Move;

//! \brief Plays a game by picking a random legal move during its turn.
class Random_AI : public Player
{
    public:
        //! \brief Picks a random legal move.
        //!
        //! \param board The current state of the board.
        //! \param clock The game clock.
        const Move& choose_move(const Board& board, const Clock& clock) const noexcept override;
        std::string name() const noexcept override;
        std::string author() const noexcept override;
};

#endif // RANDOM_AI_H
