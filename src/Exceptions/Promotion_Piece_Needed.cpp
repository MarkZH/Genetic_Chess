#include "Exceptions/Promotion_Piece_Needed.h"

#include <stdexcept>

//! Create the exception to be thrown with a message in case it is somehow not caught.
Promotion_Piece_Needed::Promotion_Piece_Needed() :
    std::runtime_error("Uncaught request for a human-specified pawn-promotion.")
{
}
