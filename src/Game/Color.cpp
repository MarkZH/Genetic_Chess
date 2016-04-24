#include "Game/Color.h"
#include "Exceptions/Generic_Exception.h"

Color opposite(Color color)
{
    if(color == NONE)
    {
        throw Generic_Exception("No opposite of NONE color.");
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
        default: throw Generic_Exception("Invalid color.");
    }
}
