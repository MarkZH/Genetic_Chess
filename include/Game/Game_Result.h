#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include <string>

#include "Color.h"

enum class Game_Result_Type
{
    ONGOING,
    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL,
    TIME_FORFEIT,
    TIME_EXPIRED_WITH_INSUFFICIENT_MATERIAL,
    OTHER
};

//! This class represents the result of a game action, primarily whether the game is still ongoing.
class Game_Result
{
    public:
        //! The default Game_Result indicates that the game has not ended.
        Game_Result() noexcept;

        //! This constructor creates a Game_Result indicating that the game has ended.
        //
        //! \param winner The color of the player that has won, or NONE if a draw.
        //! \param reason Explanation of why the game ended.
        Game_Result(Color winner, Game_Result_Type reason) noexcept;

        //! Create a Game_Result when the game has ended for an unanticipated reason.
        //
        //! \param winner The declared winner of the game.
        //! \param reason The declared reason for the end of the game.
        Game_Result(Color winner, const std::string& reason) noexcept;

        //! Indicate whether the game ended with the last action.
        bool game_has_ended() const noexcept;

        //! Returns the resultant winner (or NONE, if a draw) as a result of the last action.
        Color winner() const noexcept;

        //! Returns the reason for the game ending.
        std::string ending_reason() const noexcept;

        //! Returns the part of the PGN move annotation that goes after the # (checkmate) or + (check).
        std::string game_ending_annotation() const noexcept;

    private:
        Color victor;
        Game_Result_Type cause;
        std::string alternate_reason;
};

#endif // GAME_RESULT_H
