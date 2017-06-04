#include "Exceptions/Checkmate_Exception.h"
#include "Game/Color.h"

Checkmate_Exception::Checkmate_Exception(Color victory) :
    Game_Ending_Exception(victory, color_text(victory) + " mates")
{
}

Checkmate_Exception::~Checkmate_Exception()
{
}
