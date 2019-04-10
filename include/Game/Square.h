#ifndef SQUARE_H
#define SQUARE_H

#include <cstddef>

//! \file

//! This class represents a location on the board.
class Square
{
    public:
        Square();
        Square(char file_in, int rank_in);
        explicit Square(size_t index);

        char file() const;
        int rank() const;

        operator bool() const;

    private:
        char square_file;
        int square_rank;
};

bool operator==(const Square& a, const Square& b);
bool operator!=(const Square& a, const Square& b);

#endif // SQUARE_H
