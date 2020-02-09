#include "Game/Game_Result.h"

#include <string>

#include "Game/Color.h"
#include "Utility/String.h"

Game_Result::Game_Result() noexcept : Game_Result(NONE, Game_Result_Type::ONGOING)
{
}

Game_Result::Game_Result(Color winner, Game_Result_Type reason) noexcept :
    victor(winner),
    cause(reason),
    alternate_reason()
{
}

Game_Result::Game_Result(Color winner, const std::string& reason) noexcept :
    victor(winner),
    cause(Game_Result_Type::OTHER),
    alternate_reason(reason)
{
}

bool Game_Result::game_has_ended() const noexcept
{
    return cause != Game_Result_Type::ONGOING;
}

Color Game_Result::winner() const noexcept
{
    return victor;
}

std::string Game_Result::ending_reason() const noexcept
{
    switch(cause)
    {
        case Game_Result_Type::CHECKMATE:
            return color_text(winner()) + " mates";
        case Game_Result_Type::STALEMATE:
            return "Stalemate";
        case Game_Result_Type::FIFTY_MOVE:
            return "50-move limit";
        case Game_Result_Type::THREEFOLD_REPETITION:
            return "Threefold repetition";
        case Game_Result_Type::INSUFFICIENT_MATERIAL:
            return "Insufficient material";
        case Game_Result_Type::TIME_FORFEIT:
            return "Time forfeiture";
        case Game_Result_Type::TIME_EXPIRED_WITH_INSUFFICIENT_MATERIAL:
            return "Time expired with insufficient material";
        case Game_Result_Type::OTHER:
            return alternate_reason;
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
            return game_has_ended_by_rule() ? "1/2-1/2" : "*";
    }
}

bool Game_Result::game_has_ended_by_rule() const noexcept
{
    return game_has_ended() && cause != Game_Result_Type::OTHER;
}
