#include "Game/Color.h"

#include <string>

Piece_Color opposite(const Piece_Color color) noexcept
{
    return color == Piece_Color::WHITE ? Piece_Color::BLACK : Piece_Color::WHITE;
}

std::string color_text(const Piece_Color color) noexcept
{
    return color_text(static_cast<Winner_Color>(color));
}

std::string color_text(const Winner_Color color) noexcept
{
	switch(color)
	{
        case Winner_Color::WHITE: return "White";
        case Winner_Color::BLACK: return "Black";
        case Winner_Color::NONE:  return "None";
        default: return {};
	}
}
