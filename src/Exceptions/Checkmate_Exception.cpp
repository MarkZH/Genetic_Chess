#include "Exceptions/Checkmate_Exception.h"
#include "Game/Color.h"

Checkmate_Exception::Checkmate_Exception(Color victory) :
    Game_Ending_Exception(victory)
{
}

Checkmate_Exception::~Checkmate_Exception()
{
}

const char* Checkmate_Exception::what() const throw()
{
    return message.c_str();
}
