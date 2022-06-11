#include "Players/Proxy_Player.h"

#include <string>

Proxy_Player::Proxy_Player(const std::string& player_name) noexcept : proxy_name(player_name)
{
}

std::string Proxy_Player::name() const noexcept
{
    return proxy_name;
}
