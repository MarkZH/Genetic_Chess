#include "Exceptions/Illegal_Move_Exception.h"

Illegal_Move_Exception::Illegal_Move_Exception(const std::string& reason) : message(reason)
{
}

const char* Illegal_Move_Exception::what() const noexcept
{
    return message.c_str();
}
