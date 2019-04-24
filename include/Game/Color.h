#ifndef COLOR_H
#define COLOR_H

#include <string>

//! \file

//! Used for identifying the color of a piece, player, square, or game winner.
enum  Color : unsigned
{
    WHITE,
    BLACK,
    NONE
};

Color opposite(Color color);
std::string color_text(Color color);

#endif // COLOR_H
