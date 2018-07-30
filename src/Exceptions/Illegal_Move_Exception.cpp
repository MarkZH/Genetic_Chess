#include "Exceptions/Illegal_Move_Exception.h"

#include <stdexcept>
#include <string>

Illegal_Move_Exception::Illegal_Move_Exception(const std::string& reason) : std::runtime_error(reason)
{
}
