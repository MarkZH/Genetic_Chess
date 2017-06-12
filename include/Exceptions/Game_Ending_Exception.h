#ifndef GAME_ENDING_EXCEPTION_H
#define GAME_ENDING_EXCEPTION_H

#include <exception>

#include "Game/Color.h"

class Game_Ending_Exception : public std::exception
{
    public:
        explicit Game_Ending_Exception(Color winner_in, const std::string& message_in);

        const char* what() const throw() override;
        Color winner() const;

    private:
        Color victor;
        std::string message;
};

#endif // GAME_ENDING_EXCEPTION_H
