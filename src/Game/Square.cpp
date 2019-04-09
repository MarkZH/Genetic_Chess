#include "Game/Square.h"

#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstddef>

//! The default constructor creates an invalid square location.
Square::Square() : square_file('\0'), square_rank(0)
{
}

//! This constructor creates a user-defined square.
Square::Square(char file_in, int rank_in) : square_file(file_in), square_rank(rank_in)
{
}

//! Creates a square from an index given by Board::square_index().
Square::Square(size_t index) : square_file('a' + index/8), square_rank(1 + index%8)
{
    assert(index < 64);
}

//! The file of the square.

//! \returns The letter label of the square file.
char Square::file() const
{
    return square_file;
}

//! The rank of the square.

//! \returns The numerical label of the rank.
int Square::rank() const
{
    return square_rank;
}

//! Check if the square is valid.

//! \returns Whether the file and rank are zero (i.e., invalid).
Square::operator bool() const
{
    return file() != '\0' && rank() != 0;
}

//! Calculate the minimum number of king moves required to get from one Square to another.

//! \param a A Square.
//! \param b Another Square.
//! \returns The mimimum number of moves needed to get a king piece from the first square to the second square.
int king_distance(const Square& a, const Square& b)
{
    return std::max(std::abs(a.file() - b.file()),
                    std::abs(a.rank() - b.rank()));
}

//! Check if two squares are the same.

//! \returns Whether two squares have the same file and rank.
bool operator==(const Square& a, const Square& b)
{
    return (a.file() == b.file()) && (a.rank() == b.rank());
}

//! Check if two squares are not the same.

//! \returns Whether two squares differ in their file or rank.
bool operator!=(const Square& a, const Square& b)
{
    return !(a == b);
}
