#ifndef PROXY_PLAYER_H
#define PROXY_PLAYER_H

#include <string>

#include "Players/Random_AI.h"

//! \brief A class to hold remote opponent information when playing through a GUI or over the internet.
class Proxy_Player : public Random_AI
{
    public:
        //! Create the proxy player with a name
        //!
        //! \param player_name The name to use for the player
        explicit Proxy_Player(const std::string& player_name) noexcept;

        std::string name() const noexcept override;

    private:
        std::string proxy_name;
};

#endif // PROXY_PLAYER_H
