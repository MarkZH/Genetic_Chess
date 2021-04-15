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

Square_Color opposite(const Square_Color color) noexcept
{
    return color == Square_Color::WHITE ? Square_Color::BLACK : Square_Color::WHITE;
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
