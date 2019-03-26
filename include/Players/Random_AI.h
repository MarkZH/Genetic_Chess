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
        const Move& choose_move(const Board& board, const Clock& clock) const override;
        std::string name() const override;

    protected:
        const Move* choose_random_move(const Board& board) const;
};

#endif // RANDOM_AI_H
