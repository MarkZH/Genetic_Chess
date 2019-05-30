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
//
//! Note: Only valid for straight (rook- or bishop-type) moves
Square_Difference Square_Difference::step() const
{
    return {Math::sign(file_change), Math::sign(rank_change)};
}

//! Return the first square when iterating all squares
All_Squares_Iterator All_Squares::begin() const
{
    return All_Squares_Iterator({'a', 1});
}

//! Return an invalid Square to end iteration
All_Squares_Iterator All_Squares::end() const
{
    return All_Squares_Iterator({});
}

const Square::square_index_t Square::invalid_index = 64;

//! The default constructor creates an invalid square location.
Square::Square() : square_index(invalid_index)
{
}

//! This constructor creates a user-defined square.
//
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
//
//! \returns The letter label of the square file.
char Square::file() const
{
    return char('a' + index()/8);
}

//! The rank of the square.
//
//! \returns The numerical label of the rank.
int Square::rank() const
{
    return 1 + index()%8;
}

//! The index of the square.
//
//! \returns An unsigned integer index.
Square::square_index_t Square::index() const
{
    return square_index;
}

//! String representation of square.
std::string Square::string() const
{
    return file() + std::to_string(rank());
}

//! Returns the color of the square.
//
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
//
//! \returns Whether the square is on the Board ("i10" returns false).
bool Square::inside_board() const
{
    return index() < invalid_index;
}

//! Check whether a square has been set with a valid coordinate.
//
//! This is a synonym for Square::inside_board().
bool Square::is_set() const
{
    return inside_board();
}

//! Apply an offset to a square.
//
//! \param diff A pair of integers representing the change in file and rank
//! \returns A reference to the now-modified square. This may create a square that is off
//!          the board ("h8" + (1,1), for example).
Square& Square::operator+=(const Square_Difference& diff)
{
    assert(std::abs(diff.file_change) < 8 && std::abs(diff.rank_change) < 8);
    square_index_t new_rank_index = square_index + diff.rank_change;
    if(new_rank_index/8 != square_index/8) // make sure file did not change (happens if new square is off the board vertically)
    {
        square_index = invalid_index;
        return *this;
    }

    square_index = new_rank_index + 8*(diff.file_change);
    return *this;
}

//! Apply an offset to a square in the opposite direction.
//
//! \param diff A pair of integers representing the change in file and rank
//! \returns A reference to the now-modified square.
Square& Square::operator-=(const Square_Difference& diff)
{
    return *this += -diff;
}

//! Go to the next Square when iterating over Square::all_squares().
Square& Square::operator++()
{
    ++square_index;
    return *this;
}

//! Check that a square file is a valid value.
//
//! \param file Square file to check ('a' <= file <= 'h')
//! \returns Whether the file meets the condition according to the parameter specification above.
bool Square::inside_board(char file)
{
    return 'a' <= file && file <= 'h';
}

//! Check that a square rank is a valid value.
//
//! \param rank Square rank to check (1 <= file <= 8)
//! \returns Whether the rank meets the condition according to the parameter specification above.
bool Square::inside_board(int rank)
{
    return 1 <= rank && rank <= 8;
}

//! Check that a square file and rank is a valid value.
//
//! \param file Square file to check ('a' <= file <= 'h')
//! \param rank Square rank to check (1 <= file <= 8)
//! \returns Whether the rank meets the condition according to the parameter specification above.
bool Square::inside_board(char file, int rank)
{
    return inside_board(file) && inside_board(rank);
}

//! Returns a pseudo-container than can iterator over all square on a Board.
//
//! The primary use of this method is to replace
//! \code{cpp}
//!     for(char file = 'a'; file <= 'h'; ++file)
//!     {
//!         for(int rank = 1; rank <= 8; ++rank)
//!         {
//!             // ...
//! \endcode
//! with
//! \code{cpp}
//!     for(auto square : Square::all_squares())
//!     {
//!         // ...
//! \endcode
//!
//! Use when the order of Square iteration doesn't matter.
All_Squares Square::all_squares()
{
    return {};
}

//! Create a container of Squares between--but not including--two squares.
//
//! \param a A square that borders the collection but is not included.
//! \param b Another square that borders the collection but is not included.
//! \returns A Squares_in_a_Line instance for use in a range-for loop or \<algorithms\>.
//
//! This structure only works if the Squares in the arguments are along a common
//! row, column, or diagonal.
Squares_in_a_Line Square::squares_between(Square a, Square b)
{
    return Squares_in_a_Line(a, b);
}

//! Create a container of Squares along a line specified by a Square and a direction.
//
//! \param origin A square that starts a line of Squares but is not included in the collection.
//! \param direction The direction that the line should proceed along. There is no restriction
//!        on the direction or length of the difference. A knight move is valid.
//
//! The end of the collection is when the squares leave the Board.
Squares_in_a_Line Square::square_line_from(Square origin, Square_Difference direction)
{
    return Squares_in_a_Line(origin, direction);
}

//! Check if two squares are the same.
//
//! \returns Whether two squares have the same file and rank or are both outside the board.
bool operator==(Square a, Square b)
{
    return (a.index() == b.index()) || ( ! a.inside_board() && ! b.inside_board());
}

//! Check if two squares are not the same.
//
//! \returns Whether two squares differ in their file or rank.
bool operator!=(Square a, Square b)
{
    return !(a == b);
}

//! Add an offset to a square, returning a new Square.
//
//! \param square The original square.
//! \param diff A pair of integers representing the change in file and rank.
Square operator+(Square square, const Square_Difference& diff)
{
    return square += diff;
}

//! Add an offset to a square in the opposite direction, returning a new Square.
//
//! \param square The original square.
//! \param diff A pair of integers representing the change in file and rank.
Square operator-(Square square, const Square_Difference& diff)
{
    return square -= diff;
}

//! Find the 2-dimensional distance offset between two squares.
//
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

//! Returns whether a rook or bishop could move from one square to another in one move.
bool straight_line_move(Square start, Square end)
{
    auto move = end - start;
    if(move.file_change == 0 || move.rank_change == 0)
    {
        return true;
    }

    return std::abs(move.file_change) == std::abs(move.rank_change);
}

//! Returns whether two Square_Differences are in the exact same or exact opposite directions.
//
//! For example, (2, 2) is parallel to (3, 3) and (-1, -1)
//!
//! Note: A difference of (0, 0) is parallel with every possible other difference.
bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2)
{
    // Think of the determinant of a 2x2 matrix with the two moves as column vectors.
    // Parallel (including anti-parallel) vectors are not linearly independent, so
    // the determinant of the matrix is zero.
    return move_1.file_change*move_2.rank_change == move_2.file_change*move_1.rank_change;
}

//! Returns whether two Square_Differences are parallel in the same direction.
//
//! For example, (2, 2) and (-1, -1) are parallel but not in the same direction,
//! whereas (2, 2) and (1, 1) are parallel and in the same direction.
bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2)
{
    return moves_are_parallel(move_1, move_2) &&
           move_1.file_change*move_2.file_change + move_1.rank_change*move_2.rank_change > 0; // dot product
}

//! Construct the collection of in-line Squares.
//
//! \param start The start of the line of squares (not included in the collection).
//! \param square_step The direction of the line extending away from start.
Square_Line_Iterator::Square_Line_Iterator(Square start, const Square_Difference & square_step) :
    current_square(start),
    step(square_step)
{
}

//! Advance the iterator to the next Square in the line.
Square_Line_Iterator& Square_Line_Iterator::operator++()
{
    current_square += step;
    return *this;
}

//! Iterators are equal if they refer to the same square.
bool Square_Line_Iterator::operator==(const Square_Line_Iterator& other) const
{
    return current_square == other.current_square;
}

//! Iterators are different if they refer to different squares.
bool Square_Line_Iterator::operator!=(const Square_Line_Iterator& other) const
{
    return ! (*this == other);
}

//! Get the Square referred to by the iterator.
Square Square_Line_Iterator::operator*() const
{
    return current_square;
}

//! Create a collection of Squares between--but not including--the parameter Squares.
//
//! \param first_border A Square that borders the collection.
//! \param second_border Another Square that borders the collection.
//
//! The parameter Squares must be along a common row, column, or diagonal.
Squares_in_a_Line::Squares_in_a_Line(Square first_border, Square second_border) :
    border1(first_border),
    border2(second_border),
    step((second_border - first_border).step())
{
}

//! Create a collections of Squares along a line starting at--but not including--a Square.
//
//! \param origin The start of the line of Squares.
//! \param square_step The direction the line proceeds. There are no restrictions on the
//!        direction or step size.
Squares_in_a_Line::Squares_in_a_Line(Square origin, const Square_Difference& square_step) :
    border1(origin),
    border2{},
    step(square_step)
{
}

//! Start the Square-line collection.
Square_Line_Iterator Squares_in_a_Line::begin() const
{
    return Square_Line_Iterator(border1 + step, step);
}

//! End the Square-line collection.
Square_Line_Iterator Squares_in_a_Line::end() const
{
    return {border2, step};
}

//! Create an iterator referring to a square.
//
//! \param start The square the iterator will refer to.
All_Squares_Iterator::All_Squares_Iterator(Square start) : current_square(start)
{
}

//! Go to the next Square along the line.
All_Squares_Iterator& All_Squares_Iterator::operator++()
{
    ++current_square;
    return *this;
}

//! Iterators are equal when they refer to the same Square.
bool All_Squares_Iterator::operator==(const All_Squares_Iterator& other) const
{
    return current_square == other.current_square;
}

//! Iterators are unequal when they refer to different Squares.
bool All_Squares_Iterator::operator!=(const All_Squares_Iterator& other) const
{
    return ! (*this == other);
}

//! Get the Square referred to by the iterator.
Square All_Squares_Iterator::operator*() const
{
    return current_square;
}
