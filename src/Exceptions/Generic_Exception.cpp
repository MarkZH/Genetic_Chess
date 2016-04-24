#include "Exceptions/Generic_Exception.h"

#include <string>

Generic_Exception::Generic_Exception(std::string message) : error(message)
{
}

const char* Generic_Exception::what() const throw()
{
    return error.c_str();
}
