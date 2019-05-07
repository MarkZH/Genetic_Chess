#include "Exceptions/Game_Ended.h"

#include <stdexcept>
#include <string>

#include "Game/Color.h"
#include "Game/Game_Result.h"

//! Create a game-ending exception to throw.
//
//! \param winner_in The declared winner of the game.
//! \param message_in A textual reason for the game ending.
Game_Ended::Game_Ended(Color winner_in,
                       const std::string& message_in) :
    std::runtime_error(message_in),
    game_result(winner_in, message_in)
{
}

//! Return a game result for the rest of the program to process.
//
//! \returns A Game_Result equivalent to a normal game ending.
Game_Result Game_Ended::result() const
{
    return game_result;
}
