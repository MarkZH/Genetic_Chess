#ifndef GAME_ENDED_H
#define GAME_ENDED_H

#include <stdexcept>
#include <string>

#include "Game/Color.h"

class Game_Ended : public std::runtime_error
{
    public:
        Game_Ended(Color winner_in, const std::string& message_in);
        Color winner() const;

    private:
        Color victor;
};

#endif // GAME_ENDED_H
