#ifndef PROMOTION_PIECE_NEEDED_H
#define PROMOTION_PIECE_NEEDED_H

#include <stdexcept>

//! An exception thrown in case a human player moves a pawn to the far rank without indicating which piece to promote to.
class Promotion_Piece_Needed : public std::runtime_error
{
    public:
        //! Create the exception to be thrown with a message in case it is somehow not caught.
        Promotion_Piece_Needed();
};

#endif // PROMOTION_PIECE_NEEDED_H
