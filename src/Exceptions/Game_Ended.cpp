#include "Exceptions/Game_Ended.h"

#include <stdexcept>
#include <string>

#include "Game/Color.h"
#include "Game/Game_Result.h"

Game_Ended::Game_Ended(Color winner_in,
                       const std::string& message_in) :
    std::runtime_error(message_in),
    game_result(winner_in, message_in)
{
}

Game_Result Game_Ended::result() const
{
    return game_result;
}
