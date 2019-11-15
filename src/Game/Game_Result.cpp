#include "Game/Game_Result.h"

#include <string>

#include "Game/Color.h"
#include "Utility/String.h"

Game_Result::Game_Result() noexcept : Game_Result(NONE, ONGOING)
{
}

Game_Result::Game_Result(Color winner, Game_Result_Type reason) noexcept :
    victor(winner),
    cause(reason)
{
}

bool Game_Result::game_has_ended() const noexcept
{
    return cause != ONGOING;
}

Color Game_Result::winner() const noexcept
{
    return victor;
}

std::string Game_Result::ending_reason() const noexcept
{
    switch(cause)
    {
        case CHECKMATE:
            return color_text(winner()) + " mates";
        case STALEMATE:
            return "Stalemate";
        case FIFTY_MOVE:
            return "50-move limit";
        case THREEFOLD_REPETITION:
            return "Threefold repetition";
        case INSUFFICIENT_MATERIAL:
            return "Insufficient material";
        case TIME_FORFEIT:
            return "Time forfeiture";
        default:
            return {};
    }
}

std::string Game_Result::game_ending_annotation() const noexcept
{
    switch(winner())
    {
        case WHITE:
            return "1-0";
        case BLACK:
            return "0-1";
        default:
            return game_has_ended() ? "1/2-1/2" : "*";
    }
}

std::string Game_Result::game_record_annotation() const noexcept
{
    return cause == CHECKMATE ? "#" : "";
}
