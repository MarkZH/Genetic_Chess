#ifndef ILLEGAL_MOVE_EXCEPTION_H
#define ILLEGAL_MOVE_EXCEPTION_H

#include <stdexcept>
#include <string>

class Illegal_Move_Exception : public std::runtime_error
{
    public:
        explicit Illegal_Move_Exception(const std::string& reason);
};

#endif // ILLEGAL_MOVE_EXCEPTION_H
