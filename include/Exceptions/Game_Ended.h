#ifndef GAME_ENDED_H
#define GAME_ENDED_H

#include <stdexcept>

//! \brief An exception class that is thrown by GUI mediators to end a game session.
class Game_Ended : public std::runtime_error
{
    public:
        using std::runtime_error::runtime_error;
};

#endif // GAME_ENDED_H
