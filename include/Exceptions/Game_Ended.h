#ifndef GAME_ENDED_H
#define GAME_ENDED_H

#include <stdexcept>
#include <string>

#include "Game/Color.h"
#include "Game/Game_Result.h"

class Game_Ended : public std::runtime_error
{
    public:
        Game_Ended(Color winner_in, const std::string& message_in);
        Game_Result result() const;

    private:
        Game_Result game_result;
};

#endif // GAME_ENDED_H
