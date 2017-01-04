#include "Exceptions/Stalemate_Exception.h"

Stalemate_Exception::Stalemate_Exception(const std::string& message_in) : Game_Ending_Exception(NONE, "1/2-1/2 " + message_in)
{
}

Stalemate_Exception::~Stalemate_Exception()
{
}
