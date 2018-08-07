#ifndef PROMOTION_PIECE_NEEDED_H
#define PROMOTION_PIECE_NEEDED_H

#include <stdexcept>

class Promotion_Piece_Needed : public std::runtime_error
{
    public:
        Promotion_Piece_Needed();
};

#endif // PROMOTION_PIECE_NEEDED_H
