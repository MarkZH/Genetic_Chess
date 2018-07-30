#include "Exceptions/Game_Ending_Exception.h"

#include <stdexcept>
#include <string>

#include "Game/Color.h"

Game_Ending_Exception::Game_Ending_Exception(Color winner_in,
                                             const std::string& message_in) :
    std::runtime_error(message_in),
    victor(winner_in)
{
}

Color Game_Ending_Exception::winner() const
{
    return victor;
}
