#include "Game/Game_Result.h"

#include <string>

#include "Game/Color.h"
#include "Utility/String.h"

//! The default Game_Result indicates that the game has not ended.
Game_Result::Game_Result() : victor(NONE), cause()
{
}

//! This constructor creates a Game_Result indicating that the game has ended.
//
//! \param winner The color of the player that has won, or NONE if a draw.
//! \param reason Explanation of why the game ended.
Game_Result::Game_Result(Color winner,
                         const std::string& reason) :
                             victor(winner),
                             cause(reason)
{
}

//! Indicate whether the game ended with the last action.
bool Game_Result::game_has_ended() const
{
    return ! cause.empty();
}

//! Returns the resultant winner (or NONE, if a draw) as a result of the last action.
Color Game_Result::winner() const
{
    return victor;
}

//! Returns the reason for the game ending.
std::string Game_Result::ending_reason() const
{
    return cause;
}

//! Returns the part of the PGN move annotation that goes after the # (checkmate) or + (check).
std::string Game_Result::game_ending_annotation() const
{
    switch(winner())
    {
        case WHITE:
            return "1-0";
        case BLACK:
            return "0-1";
        default:
            return game_has_ended() ? "1/2-1/2" : "";
    }
}

//! Returns the winner annotation to be appended to the last move of a PGN-style game record.
std::string Game_Result::game_record_annotation() const
{
    if(game_has_ended())
    {
        std::string prefix = " ";
        if(String::ends_with(cause, "mates"))
        {
            prefix =  "#" + prefix;
        }
        return prefix + game_ending_annotation();
    }
    else
    {
        return {};
    }
}
