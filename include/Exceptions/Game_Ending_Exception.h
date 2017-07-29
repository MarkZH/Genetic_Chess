#ifndef GAME_ENDING_EXCEPTION_H
#define GAME_ENDING_EXCEPTION_H

#include <exception>

#include "Game/Color.h"

class Game_Ending_Exception : public std::exception
{
    public:
        Game_Ending_Exception(Color winner_in, const std::string& message_in);

        const char* what() const noexcept override;
        Color winner() const;

    private:
        Color victor;
        std::string message;
};

#endif // GAME_ENDING_EXCEPTION_H
