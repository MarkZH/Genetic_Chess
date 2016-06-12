#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Board;
class Clock;
class Complete_Move;
class Game_Ending_Exception;

class Player
{
    public:
        virtual ~Player();

        virtual const Complete_Move choose_move(const Board& board, const Clock& clock) const = 0;
        virtual std::string name() const = 0;

        virtual void process_game_ending(const Game_Ending_Exception& gee) const;
};

#endif // PLAYER_H
