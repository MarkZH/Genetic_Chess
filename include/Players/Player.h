#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Board;
class Clock;
class Move;
class Game_Result;

class Player
{
    public:
        virtual ~Player() = default;

        virtual const Move& choose_move(const Board& board, const Clock& clock) const = 0;
        virtual std::string name() const = 0;
        virtual std::string author() const;

        virtual void process_game_ending(const Game_Result& ending, const Board& board) const;
        virtual std::string commentary_for_next_move(const Board& board) const;

        virtual void initial_board_setup(Board& board) const;
        virtual bool stop_for_local_clock() const;
};

#endif // PLAYER_H
