#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

//! \brief An exception class that is thrown by GUI mediators to end a game session.
class Game_Ended : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


//! \brief An exception thrown when a Genetic_AI cannot be successfully constructed.
class Genetic_AI_Creation_Error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


//! \brief An exception indicating an illegal move is being attempted.
//!
//! This is thrown while interpretting the textual representation of
//! a chess move if it is found to be invalid or illegal according
//! to the current board state.
class Illegal_Move : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#endif // EXCEPTIONS_H
