#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Board;
class Clock;
class Move;

//! This class represents chess players and encapsulates move-choosing methods.
class Player
{
    public:
        virtual ~Player() = default;

        //! The player chooses the next move in the game.
        //
        //! The classes derived from Player implement the various algorithms for
        //! choosing moves here.
        //! \param board The current board position. The players choose the move from board.legal_moves().
        //! \param clock The game clock--allowing the player to decide how much time to spend choosing a move.
        virtual const Move& choose_move(const Board& board, const Clock& clock) const = 0;

        //! Think when it is not the Player's turn to move
        //
        //! \param board The board to ponder on (just after this player's last move).
        //! \param clock The game clock for guessing how much time the opponent will use.
        //! \param thinking_allowed Allow thinking about the next move during opponent's move.
        virtual void ponder(const Board& board, const Clock& clock, bool thinking_allowed) const;

        //! The name of the player.
        //
        //! \returns The name of the individual player. This may have specific details like ID numbers
        //!          or the name of a human player.
        virtual std::string name() const = 0;

        //! Returns the name of the author of the chess engine.
        virtual std::string author() const = 0;

        //! Provide commentary for the move to come given a past state of the game.
        //
        //! \param board The board position prior to the move the player should comment on.
        //! \param move_number The numeric label of the move (often 1 for the first move of the white and black player).
        //!        This is also used when the game record begins in the middle of a game.
        //! \returns A text string commenting on the move that will follow the input position.
        virtual std::string commentary_for_next_move(const Board& board, size_t move_number) const;

        //! Get the name of this player's opponent when playing through a GUI.
        //
        //! \param name The GUI-provided name of the opponent.
        void set_opponent_name(const std::string& name) const;

        //! Provide the name of the opponent if the opponent cannot do so itself.
        //
        //! If playing through a GUI, the opponent will not be a part of the same process
        //! as this player.
        std::string opponent_name() const;

        //! Use the short version of CECP thinking output.
        //
        //! Some GUIs cannot handle the optional data.
        static void set_short_post();

        //! Check whether to use the short version of CECP thinking output.
        static bool use_short_post();

    private:
        mutable std::string opposing_player_name;
        static bool use_short_cecp_post;
};

#endif // PLAYER_H
