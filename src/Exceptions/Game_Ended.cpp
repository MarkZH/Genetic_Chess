#include "Exceptions/Game_Ended.h"

#include <stdexcept>

Game_Ended::Game_Ended() noexcept : std::runtime_error("Uncaught Game_Ended exception.")
{
}
