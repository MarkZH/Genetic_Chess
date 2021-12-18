#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "Players/Thinking.h"

class Board;
class Clock;
class Move;

//! \brief This class represents chess players and encapsulates move-choosing methods.
class Player
{
    public:
        virtual ~Player() = default;

        //! \brief The player chooses the next move in the game.
        //!
        //! The classes derived from Player implement the various algorithms for
        //! choosing moves here.
        //! \param board The current board position. The players choose the move from board.legal_moves().
        //! \param clock The game clock--allowing the player to decide how much time to spend choosing a move.
        virtual const Move& choose_move(const Board& board, const Clock& clock) const noexcept = 0;

        //! \brief Reset player internals (if any) for a new game
        virtual void reset() const noexcept;

        //! \brief The name of the player.
        //!
        //! \returns The name of the individual player. This may have specific details like ID numbers
        //!          or the name of a human player.
        virtual std::string name() const noexcept = 0;

        //! \brief Returns the name of the author of the chess engine.
        virtual std::string author() const noexcept = 0;

        //! \brief Provide commentary for the move to come given a past state of the game.
        //!
        //! \param board The board position prior to the move the player should comment on.
        //! \param move_number The numeric label of the move (often 1 for the first move of the white and black player).
        //!        This is also used when the game record begins in the middle of a game.
        //! \returns A text string commenting on the move that will follow the input position.
        virtual std::string commentary_for_next_move(const Board& board, size_t move_number) const noexcept;

        //! \brief Remove the data for the last move from the chess engine.
        //!
        //! \param last_move The last move made on the board for use by the internals of the chess engine.
        virtual void undo_move(const Move* last_move) const noexcept;

        //! \brief Set the format an engine should output while picking a move.
        //!
        //! \param mode Which chess engine protocol is being used: CECP, UCI, or NO_THINKING.
        static void set_thinking_mode(Thinking_Output_Type) noexcept;

        //! \brief Force the Player to stop thinking and immediately make a move.
        static void pick_move_now() noexcept;

        //! \brief Allow the Player to take any amount of time to choose a move.
        static void choose_move_at_leisure() noexcept;

        //! \brief Record a win for this AI.
        void add_win() noexcept;

        //! \brief Get win count for this AI.
        int wins() const noexcept;

        //! \brief Record a draw for this AI.
        void add_draw() noexcept;

        //! \brief Get draw count for this AI.
        int draws() const noexcept;

    protected:
        //! \brief Find out what kind of format an engine should output while picking a move.
        //!
        //! \returns Format of thinking output: CECP, UCI, or NO_THINKING.
        static Thinking_Output_Type thinking_mode() noexcept;

        //! \brief Check whether a Player should stop thinking and immediately move.
        static bool must_pick_move_now() noexcept;

    private:
        int win_count = 0;
        int draw_count = 0;
};

#endif // PLAYER_H
