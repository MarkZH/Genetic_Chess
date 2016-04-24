#ifndef COLOR_H
#define COLOR_H

#include <string>

enum Color {WHITE, BLACK, NONE};

Color opposite(Color color);
std::string color_text(Color color);

#endif // COLOR_H
