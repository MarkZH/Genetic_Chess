#ifndef ILLEGAL_MOVE_H
#define ILLEGAL_MOVE_H

#include <stdexcept>

//! An exception indicating an illegal move is being attempted.

//! This is thrown while interpretting the textual representation of
//! a chess move if it is found to be invalid or illegal according
//! to the current board state.
class Illegal_Move : public std::runtime_error
{
    public:
        using std::runtime_error::runtime_error;
};

#endif // ILLEGAL_MOVE_H
