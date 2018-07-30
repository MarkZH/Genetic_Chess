#ifndef GAME_ENDING_EXCEPTION_H
#define GAME_ENDING_EXCEPTION_H

#include <stdexcept>

#include "Game/Color.h"

class Game_Ending_Exception : public std::runtime_error
{
    public:
        Game_Ending_Exception(Color winner_in, const std::string& message_in);
        Color winner() const;

    private:
        Color victor;
};

#endif // GAME_ENDING_EXCEPTION_H
