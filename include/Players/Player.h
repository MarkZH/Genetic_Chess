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

        virtual void process_game_ending(const Game_Result& gee, const Board& board) const;
        virtual std::string get_commentary_for_move(size_t move_number) const;

        virtual void initial_board_setup(Board& board) const;
};

#endif // PLAYER_H
