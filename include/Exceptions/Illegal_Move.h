#ifndef ILLEGAL_MOVE_H
#define ILLEGAL_MOVE_H

#include <stdexcept>
#include <string>

class Illegal_Move : public std::runtime_error
{
    public:
        explicit Illegal_Move(const std::string& reason);
};

#endif // ILLEGAL_MOVE_H
