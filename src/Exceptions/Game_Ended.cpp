#include "Exceptions/Game_Ended.h"

#include <stdexcept>

//! Create a game-ending exception to throw.
Game_Ended::Game_Ended() : std::runtime_error("Uncaught Game_Ended exception.")
{
}
