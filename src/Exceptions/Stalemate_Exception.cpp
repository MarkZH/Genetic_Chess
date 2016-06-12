#include "Exceptions/Stalemate_Exception.h"

Stalemate_Exception::Stalemate_Exception(std::string message) : Game_Ending_Exception(NONE),
                                                                reason("1/2-1/2 " + message)
{
}

Stalemate_Exception::~Stalemate_Exception()
{
}

const char* Stalemate_Exception::what() const throw()
{
    return reason.c_str();
}
