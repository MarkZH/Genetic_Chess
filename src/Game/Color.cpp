#include <stdexcept>

#include "Game/Color.h"

Color opposite(Color color)
{
    if(color == NONE)
    {
        throw std::runtime_error("No opposite of NONE color.");
    }
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
