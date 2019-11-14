#ifndef COLOR_H
#define COLOR_H

#include <string>

//! \file

//! Used for identifying the color of a piece, player, square, or game winner.
enum Color : unsigned
{
    WHITE,
    BLACK,
    NONE
};

//! Returns the opposite of the input color: BLACK to WHITE or WHITE to BLACK.
//
//! \param color The color to reverse. If called with NONE, it will trip a DEBUG assert or return WHITE in RELEASE builds.
//! \returns Opposite color.
Color opposite(Color color);

//! Returns a text (std::string) version of a color.
//
//! \param color The color to convert to text.
//! \returns A textual representation of the color.
//! \throws std::runtime_error if an invalid Color value is passed in.
std::string color_text(Color color);

#endif // COLOR_H
