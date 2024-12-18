#include "Game/Game_Result.h"

#include <string>
#include <format>

#include "Game/Color.h"

Game_Result::Game_Result() noexcept : Game_Result(Winner_Color::NONE, Game_Result_Type::ONGOING)
{
}

Game_Result::Game_Result(const Winner_Color winner, const Game_Result_Type reason) noexcept :
    victor(winner),
    cause(reason),
    alternate_reason(),
    shutdown_program(false)
{
}

Game_Result::Game_Result(const Piece_Color winner, const Game_Result_Type reason) noexcept :
    Game_Result(static_cast<Winner_Color>(winner), reason)
{
}

Game_Result::Game_Result(const Winner_Color winner, const std::string& reason) noexcept :
    victor(winner),
    cause(Game_Result_Type::OTHER),
    alternate_reason(reason),
    shutdown_program(false)
{
}

Game_Result Game_Result::shutdown(const std::string& reason) noexcept
{
    auto result = Game_Result(Winner_Color::NONE, reason);
    result.shutdown_program = true;
    return result;
}

bool Game_Result::game_has_ended() const noexcept
{
    return cause != Game_Result_Type::ONGOING;
}

Winner_Color Game_Result::winner() const noexcept
{
    return victor;
}

std::string Game_Result::ending_reason() const noexcept
{
    switch(cause)
    {
        case Game_Result_Type::CHECKMATE:
            return std::format("{} mates", color_text(winner()));
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
        case Winner_Color::WHITE:
            return "1-0";
        case Winner_Color::BLACK:
            return "0-1";
        default:
            return game_has_ended_by_rule() ? "1/2-1/2" : "*";
    }
}

bool Game_Result::game_has_ended_by_rule() const noexcept
{
    return game_has_ended() && cause != Game_Result_Type::OTHER;
}

bool Game_Result::exit_program() const noexcept
{
    return shutdown_program;
}
