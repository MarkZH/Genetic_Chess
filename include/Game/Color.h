#ifndef COLOR_H
#define COLOR_H

#include <string>

enum Color {WHITE = 0,
            BLACK = 1,
            NONE}; // values for use as indices

Color opposite(Color color);
std::string color_text(Color color);

#endif // COLOR_H
