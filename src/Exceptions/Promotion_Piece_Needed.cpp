#include "Exceptions/Promotion_Piece_Needed.h"

#include <stdexcept>

Promotion_Piece_Needed::Promotion_Piece_Needed() :
    std::runtime_error("Uncaught request for a human-specified pawn-promotion.")
{
}
