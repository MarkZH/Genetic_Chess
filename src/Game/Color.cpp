#include "Game/Color.h"

#include <cassert>
#include <string>

Color opposite(Color color) noexcept
{
    assert(color == WHITE || color == BLACK);
    return static_cast<Color>(1 - color);
}

std::string color_text(Color color) noexcept
{
    switch(color)
    {
        case WHITE: return "White";
        case BLACK: return "Black";
        case NONE:  return "None";
        default:    return {};
    }
}
