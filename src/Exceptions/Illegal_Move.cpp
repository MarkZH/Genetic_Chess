#include "Exceptions/Illegal_Move.h"

#include <stdexcept>
#include <string>

Illegal_Move::Illegal_Move(const std::string& reason) :
    std::runtime_error(reason)
{
}
