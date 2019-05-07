#ifndef GAME_ENDED_H
#define GAME_ENDED_H

#include <stdexcept>
#include <string>

#include "Game/Color.h"
#include "Game/Game_Result.h"

//! An exception class that is thrown for irregular game endings.
//
//! An irregular game ending means one not caused by submitting a
//! game-ending move to a Board or punching a game Clock with time
//! exprired.
class Game_Ended : public std::runtime_error
{
    public:
        Game_Ended(Color winner_in, const std::string& message_in);
        Game_Result result() const;

    private:
        Game_Result game_result;
};

#endif // GAME_ENDED_H
