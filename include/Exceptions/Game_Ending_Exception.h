#ifndef GAME_ENDING_EXCEPTION_H
#define GAME_ENDING_EXCEPTION_H

#include <exception>

#include "Game/Color.h"

class Game_Ending_Exception : public std::exception
{
    public:
        explicit Game_Ending_Exception(Color winner_in);
        ~Game_Ending_Exception() override;

        const char* what() const throw() override = 0;
        Color winner() const;

    private:
        Color victor;
};

#endif // GAME_ENDING_EXCEPTION_H
