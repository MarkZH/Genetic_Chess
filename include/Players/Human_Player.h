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
        //! The Human_Player constructor asks for the human's name.
        //
        //! The name is optional and can be left blank.
        Human_Player();

        //! The interface for the human player is a text console.
        //
        //! The player can type moves in PGN or coordinate notation.
        //! The text-based UI includes the time left. The player can enter a blank or illegal move
        //! to update the time. If an illegal or invalid move is entered, the user is asked again
        //! to type a move.
        //! \param board The current state of the game.
        //! \param clock The amount of time left in the game.
        const Move& choose_move(const Board& board, const Clock& clock) const override;

        //! \returns The player's name, if entered.
        std::string name() const override;

        //! \returns The author of this interface.
        std::string author() const override;

    private:
        std::string player_name;
};

#endif // HUMAN_PLAYER_H
