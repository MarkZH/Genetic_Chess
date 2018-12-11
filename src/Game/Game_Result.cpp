#include "Game/Game_Result.h"

#include <string>

#include "Game/Color.h"
#include "Utility.h"

Game_Result::Game_Result() : victor(NONE), cause()
{
}

Game_Result::Game_Result(Color winner,
                         const std::string& reason) :
                             victor(winner),
                             cause(reason)
{
}

bool Game_Result::game_has_ended() const
{
    return ! cause.empty();
}

Color Game_Result::winner() const
{
    return victor;
}

std::string Game_Result::ending_reason() const
{
    return cause;
}

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

std::string Game_Result::game_record_annotation() const
{
    if(game_has_ended())
    {
        std::string prefix = "\t";
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
