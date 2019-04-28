#include "Game/Square.h"

#include <cstddef>
#include <string>
#include <cassert>
#include <cmath>

#include "Game/Color.h"

#include "Utility/Math.h"

//! Reverse the direction of a square offset.
Square_Difference Square_Difference::operator-() const
{
    return {-file_change, -rank_change};
}

//! Returns a single-step version of the offset--i.e., (-3, 3) --> (-1, 1)

//! Note: Only valid for straight (rook- or bishop-type) moves
Square_Difference Square_Difference::step() const
{
    return {Math::sign(file_change), Math::sign(rank_change)};
}

//! Return the first square when iterating all squares
Square Square_Iterator::begin() const
{
    return Square{'a', 1};
}

//! Return an invalid Square to end iteration
Square Square_Iterator::end() const
{
    return Square{};
}

unsigned int Square::invalid_index = 64;

//! The default constructor creates an invalid square location.
Square::Square() : square_index(invalid_index)
{
}

//! This constructor creates a user-defined square.

//! \param file The file (column) of the square (a-h).
//! \param rank The rank (row) of the square (1-8).
//!
//! The validity of the square coordinates is not checked in release builds.
//! In debug builds, invalid square coordinates (e.g., "i9") trigger an assertion failure.
Square::Square(char file, int rank) : square_index(8*(file - 'a') + (rank - 1))
{
    assert(inside_board());
}

//! The file of the square.

//! \returns The letter label of the square file.
char Square::file() const
{
    return char('a' + index()/8);
}

//! The rank of the square.

//! \returns The numerical label of the rank.
int Square::rank() const
{
    return 1 + index()%8;
}

//! The index of the square.

//! \returns An unsigned integer index.
size_t Square::index() const
{
    return square_index;
}

//! String representation of square.
std::string Square::string() const
{
    return file() + std::to_string(rank());
}

//! Returns the color of the square.

//! This function is useful for Board::enough_material_to_checkmate() since one
//! condition is that bishops on opposite color squares are sufficient to checkmate,
//! while bishops on the same color square are not.
//! \returns Color of square on board ("a1" --> BLACK; "h1" --> WHITE)
Color Square::color() const
{
    //return (file() - 'a')%2 == (rank() - 1)%2 ? BLACK : WHITE;
    return (index()/8)%2 == (index()%8)%2 ? BLACK : WHITE;
}

//! Check if the square is a valid Board position.

//! \returns Whether the square is on the Board ("i10" returns false).
bool Square::inside_board() const
{
    return index() < invalid_index;
}

//! Check whether a square has been set with a valid coordinate.

//! This is a synonym for Square::inside_board().
bool Square::is_set() const
{
    return inside_board();
}

//! Apply an offset to a square.

//! \param diff A pair of integers representing the change in file and rank
//! \returns A reference to the now-modified square. This may create a square that is off
//!          the board ("h8" + (1,1), for example).
Square& Square::operator+=(const Square_Difference& diff)
{
    assert(std::abs(diff.file_change) < 8 && std::abs(diff.rank_change) < 8);
    auto new_rank_index = square_index + diff.rank_change;
    if(new_rank_index/8 != square_index/8) // make sure file did not change (happens if new square is off the board vertically)
    {
        square_index = invalid_index;
        return *this;
    }

    square_index = new_rank_index + 8*(diff.file_change);
    return *this;
}

//! Apply an offset to a square in the opposite direction.

//! \param diff A pair of integers representing the change in file and rank
//! \returns A reference to the now-modified square.
Square& Square::operator-=(const Square_Difference& diff)
{
    return *this += -diff;
}

Square& Square::operator++()
{
    ++square_index;
    return *this;
}

Square Square::operator*() const
{
    return *this;
}

//! Check that a square file is a valid value.

//! \param file Square file to check ('a' <= file <= 'h')
//! \returns Whether the file meets the condition according to the parameter specification above.
bool Square::inside_board(char file)
{
    return 'a' <= file && file <= 'h';
}

//! Check that a square rank is a valid value.

//! \param rank Square rank to check (1 <= file <= 8)
//! \returns Whether the rank meets the condition according to the parameter specification above.
bool Square::inside_board(int rank)
{
    return 1 <= rank && rank <= 8;
}

//! Check that a square file and rank is a valid value.

//! \param file Square file to check ('a' <= file <= 'h')
//! \param rank Square rank to check (1 <= file <= 8)
//! \returns Whether the rank meets the condition according to the parameter specification above.
bool Square::inside_board(char file, int rank)
{
    return inside_board(file) && inside_board(rank);
}

Square_Iterator Square::all_squares()
{
    return {};
}

//! Check if two squares are the same.

//! \returns Whether two squares have the same file and rank or are both outside the board.
bool operator==(Square a, Square b)
{
    return (a.index() == b.index()) || ( ! a.inside_board() && ! b.inside_board());
}

//! Check if two squares are not the same.

//! \returns Whether two squares differ in their file or rank.
bool operator!=(Square a, Square b)
{
    return !(a == b);
}

//! Add an offset to a square, returning a new Square.

//! \param square The original square.
//! \param diff A pair of integers representing the change in file and rank.
Square operator+(Square square, const Square_Difference& diff)
{
    return square += diff;
}

//! Add an offset to a square in the opposite direction, returning a new Square.

//! \param square The original square.
//! \param diff A pair of integers representing the change in file and rank.
Square operator-(Square square, const Square_Difference& diff)
{
    return square -= diff;
}

//! Find the 2-dimensional distance offset between two squares.

//! \param a The first square.
//! \param b The second square.
//! \returns A pair of integers representing the signed difference in file and rank.
//!
//! For all valid squares on a board, a + (b - a) == b.
Square_Difference operator-(Square a, Square b)
{
    assert(a.inside_board() && b.inside_board());
    return {a.file() - b.file(), a.rank() - b.rank()};
}
