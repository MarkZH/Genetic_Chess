#include "Game/Game_Result.h"

#include <string>

#include "Game/Color.h"

Game_Result::Game_Result() : victor(NONE), cause(), out_of_time_ending(false)
{
}

Game_Result::Game_Result(Color winner,
                         const std::string& reason,
                         bool end_by_clock) :
                             victor(winner),
                             cause(reason),
                             out_of_time_ending(end_by_clock)
{
}

bool Game_Result::game_has_ended() const
{
    return ! cause.empty();
}

Color Game_Result::get_winner() const
{
    return victor;
}

std::string Game_Result::get_ending_reason() const
{
    return cause;
}

std::string Game_Result::get_game_ending_annotation() const
{
    if(victor == WHITE)
    {
        return "1-0";
    }
    else if(victor == BLACK)
    {
        return "0-1";
    }
    else
    {
        return "1/2-1/2";
    }
}

std::string Game_Result::get_game_record_annotation() const
{
    if(game_has_ended())
    {
        std::string prefix = "\t";
        if( ! out_of_time_ending && get_winner() != NONE)
        {
            prefix =  "#" + prefix;
        }
        return prefix + get_game_ending_annotation();
    }
    else
    {
        return {};
    }
}
