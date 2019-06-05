#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "Player.h"

#include <string>

class Move;
class Board;
class Clock;

//! A text-based interface for a human to type chess moves at a terminal.
class Human_Player : public Player
{
    public:
        Human_Player();

        const Move& choose_move(const Board& b, const Clock& clock) const override;
        std::string name() const override;

    private:
        std::string player_name;
};

#endif // HUMAN_PLAYER_H
