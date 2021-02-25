#include "Players/Proxy_Player.h"

#include "Game/Board.h"

#include <string>

Proxy_Player::Proxy_Player(const std::string& player_name) noexcept : proxy_name(player_name)
{
}

std::string Proxy_Player::name() const noexcept
{
    return proxy_name;
}

std::string Proxy_Player::author() const noexcept
{
    return {};
}

const Move& Proxy_Player::choose_move(const Board& board, const Clock&) const noexcept
{
    return *board.legal_moves().front();
}
