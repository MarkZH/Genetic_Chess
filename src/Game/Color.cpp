#include <stdexcept>
#include <cassert>

#include "Game/Color.h"

Color opposite(Color color)
{
    assert(color != NONE);
    return (color == WHITE ? BLACK : WHITE);
}

std::string color_text(Color color)
{
    switch(color)
    {
        case WHITE: return "White";
        case BLACK: return "Black";
        case NONE:  return "None";
        default: throw std::runtime_error("Invalid color.");
    }
}
