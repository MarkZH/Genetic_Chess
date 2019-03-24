#include "Game/Color.h"

#include <cassert>
#include <stdexcept>
#include <string>

//! \file

//! Returns the opposite of the input color: BLACK to WHITE or WHITE to BLACK.

//! If called with NONE, it will trip a debug assert or return WHITE in release builds.
Color opposite(Color color)
{
    assert(color != NONE);
    return (color == WHITE ? BLACK : WHITE);
}

//! Returns a text (std::string) version of a color.
std::string color_text(Color color)
{
    switch(color)
    {
        case WHITE: return "White";
        case BLACK: return "Black";
        case NONE:  return "None";
        default: throw std::runtime_error("Invalid color in argument of color_text().");
    }
}
