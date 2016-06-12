#include "Exceptions/Generic_Exception.h"

#include <string>

Generic_Exception::Generic_Exception(std::string message) : error(message)
{
}

Generic_Exception::~Generic_Exception()
{
}

const char* Generic_Exception::what() const throw()
{
    return error.c_str();
}
