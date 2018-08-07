#include "Exceptions/Game_Ended.h"

#include <stdexcept>
#include <string>

#include "Game/Color.h"

Game_Ended::Game_Ended(Color winner_in,
                       const std::string& message_in) :
    std::runtime_error(message_in),
    victor(winner_in)
{
}

Color Game_Ended::winner() const
{
    return victor;
}
