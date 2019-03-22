#ifndef ILLEGAL_MOVE_H
#define ILLEGAL_MOVE_H

#include <stdexcept>
#include <string>

class Illegal_Move : public std::runtime_error
{
    public:
        using std::runtime_error::runtime_error;
};

#endif // ILLEGAL_MOVE_H
