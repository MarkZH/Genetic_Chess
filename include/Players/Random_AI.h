#ifndef RANDOM_AI_H
#define RANDOM_AI_H

#include <string>

#include "Player.h"

class Board;
class Clock;
class Move;

//! Plays a game by picking a random legal move during its turn.
class Random_AI : public Player
{
    public:
        //! Picks a random legal move.
        //
        //! \param board The current state of the board.
        //! \param clock The game clock.
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        std::string name() const override;
        std::string author() const override;

    protected:
        //! Implements the random move choice.
        //
        //! \param board The current board. The move is a random entry from board.legal_moves().
        const Move* choose_random_move(const Board& board) const;
};

#endif // RANDOM_AI_H
