#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include <string>

#include "Color.h"

enum Game_Result_Type
{
    ONGOING,
    CHECKMATE,
    STALEMATE,
    FIFTY_MOVE,
    THREEFOLD_REPETITION,
    INSUFFICIENT_MATERIAL,
    TIME_FORFEIT
};

//! This class represents the result of a game action, primarily whether the game is still ongoing.
class Game_Result
{
    public:
        //! The default Game_Result indicates that the game has not ended.
        Game_Result();
        
        //! This constructor creates a Game_Result indicating that the game has ended.
        //
        //! \param winner The color of the player that has won, or NONE if a draw.
        //! \param reason Explanation of why the game ended.
        Game_Result(Color winner, Game_Result_Type reason);

        //! Indicate whether the game ended with the last action.
        bool game_has_ended() const;
        
        //! Returns the resultant winner (or NONE, if a draw) as a result of the last action.
        Color winner() const;
        
        //! Returns the reason for the game ending.
        std::string ending_reason() const;
        
        //! Returns the part of the PGN move annotation that goes after the # (checkmate) or + (check).
        std::string game_ending_annotation() const;
        
        //! Returns the winner annotation to be appended to the last move of a PGN-style game record.
        std::string game_record_annotation() const;

    private:
        Color victor;
        Game_Result_Type cause;
};

#endif // GAME_RESULT_H
