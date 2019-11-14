#ifndef GAME_ENDED_H
#define GAME_ENDED_H

#include <stdexcept>

//! An exception class that is thrown by GUI mediators to end a game session.
class Game_Ended : public std::runtime_error
{
    public:
        //! Create a game-ending exception to throw.
        Game_Ended() noexcept;
};

#endif // GAME_ENDED_H
