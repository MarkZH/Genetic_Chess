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

//! \brief This class represents the result of a game action, primarily whether the game is still ongoing.
class Game_Result
{
    public:
        //! \brief The default Game_Result indicates that the game has not ended.
        Game_Result() noexcept;

        //! \brief This constructor creates a Game_Result indicating that the game has ended.
        //!
        //! \param winner The color of the player that has won, or Winner_Color::NONE if a draw.
        //! \param reason Explanation of why the game ended.
        Game_Result(Winner_Color winner, Game_Result_Type reason) noexcept;

        //! \brief This constructor creates a Game_Result based on the color of the winning player.
        //!
        //! \param winner The color of the pieces of the player who won.
        //! \param reason Explanation for why the game ended.
        Game_Result(Piece_Color winner, Game_Result_Type reason) noexcept;

        //! \brief Create a Game_Result when the game has ended for an unanticipated reason.
        //!
        //! \param winner The declared winner of the game.
        //! \param reason The declared reason for the end of the game.
        //! \param shutdown Whether this result should shutdown the program.
        Game_Result(Winner_Color winner, const std::string& reason, bool shutdown) noexcept;

        //! \brief Indicate whether the game ended with the last action.
        bool game_has_ended() const noexcept;

        //! \brief Returns the resultant winner (or Winner_Color::NONE, if a draw) as a result of the last action.
        Winner_Color winner() const noexcept;

        //! \brief Returns the reason for the game ending.
        std::string ending_reason() const noexcept;

        //! \brief Returns the part of the PGN move annotation that goes after the # (checkmate) or + (check).
        std::string game_ending_annotation() const noexcept;

        //! \brief Returns whether or not the program should shutdown after getting this result.
        bool exit_program() const noexcept;

    private:
        Winner_Color victor;
        Game_Result_Type cause;
        std::string alternate_reason;
        bool shutdown_program;

        bool game_has_ended_by_rule() const noexcept;
};

#endif // GAME_RESULT_H
