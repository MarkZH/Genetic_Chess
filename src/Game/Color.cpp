#include "Game/Color.h"

#include <cassert>
#include <stdexcept>
#include <string>

//! \file

//! Returns the opposite of the input color: BLACK to WHITE or WHITE to BLACK.

//! \param color The color to reverse. If called with NONE, it will trip a DEBUG assert or return WHITE in RELEASE builds.
//! \returns Opposite color.
Color opposite(Color color)
{
    assert(color != NONE);
    return (color == WHITE ? BLACK : WHITE);
}

//! Returns a text (std::string) version of a color.

//! \param color The color to convert to text.
//! \returns A textual representation of the color.
//! \throws std::runtime_error if an invalid Color value is passed in.
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
