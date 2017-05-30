#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Board;
class Clock;
struct Complete_Move;
class Game_Ending_Exception;

class Player
{
    public:
        virtual ~Player();

        virtual const Complete_Move choose_move(const Board& board, const Clock& clock) const = 0;
        virtual std::string name() const = 0;
        virtual std::string author() const;

        virtual void process_game_ending(const Game_Ending_Exception& gee, const Board& board) const;
        virtual std::string get_commentary_for_move(size_t move_number) const;
};

#endif // PLAYER_H
