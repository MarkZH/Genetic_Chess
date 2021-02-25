#ifndef PROXY_PLAYER_H
#define PROXY_PLAYER_H

#include <string>

#include "Players/Player.h"

class Board;
class Move;
class Clock;

//! \brief A class to hold remote opponent information when playing through a GUI or over the internet.
class Proxy_Player : public Player
{
    public:
        //! Create the proxy player with a name
        //!
        //! \param player_name The name to use for the player
        Proxy_Player(const std::string& player_name) noexcept;

        std::string name() const noexcept override;
        std::string author() const noexcept override;

        const Move& choose_move(const Board& board, const Clock& clock) const noexcept override;

    private:
        std::string proxy_name;
};

#endif // PROXY_PLAYER_H
