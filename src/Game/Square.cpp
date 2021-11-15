#include "Game/Square.h"

#include <cstddef>
#include <string>
#include <cassert>
#include <cmath>

#include "Game/Color.h"

#include "Utility/Math.h"

Square_Difference Square_Difference::operator-() const noexcept
{
    return {-file_change, -rank_change};
}

Square_Difference Square_Difference::step() const noexcept
{
    return {Math::sign(file_change), Math::sign(rank_change)};
}

All_Squares_Iterator All_Squares::begin() const noexcept
{
    return All_Squares_Iterator({'a', 1});
}

All_Squares_Iterator All_Squares::end() const noexcept
{
    return All_Squares_Iterator({});
}

const Square::square_index_t Square::invalid_index = 64;

Square::Square() noexcept : square_index(invalid_index)
{
}

Square::Square(const char file, const int rank) noexcept : square_index(8*(file - 'a') + (rank - 1))
{
    assert(inside_board());
}

char Square::file() const noexcept
{
    return char('a' + index()/8);
}

int Square::rank() const noexcept
{
    return 1 + index()%8;
}

Square::square_index_t Square::index() const noexcept
{
    return square_index;
}

std::string Square::text() const noexcept
{
    return file() + std::to_string(rank());
}

Square_Color Square::color() const noexcept
{
    //return (file() - 'a')%2 == (rank() - 1)%2 ? Square_Color::BLACK : Square_Color::WHITE;
    return (index()/8)%2 == (index()%8)%2 ? Square_Color::BLACK : Square_Color::WHITE;
}

bool Square::inside_board() const noexcept
{
    return index() < invalid_index;
}

bool Square::is_set() const noexcept
{
    return inside_board();
}

Square& Square::operator+=(const Square_Difference& diff) noexcept
{
    assert(std::abs(diff.file_change) < 8 && std::abs(diff.rank_change) < 8);
    const square_index_t new_rank_index = square_index + diff.rank_change;
    if(new_rank_index/8 != square_index/8) // make sure file did not change (happens if new square is off the board vertically)
    {
        square_index = invalid_index;
        return *this;
    }

    square_index = new_rank_index + 8*(diff.file_change);
    return *this;
}

Square& Square::operator-=(const Square_Difference& diff) noexcept
{
    return *this += -diff;
}

Square& Square::operator++() noexcept
{
    ++square_index;
    return *this;
}

bool Square::inside_board(const char file) noexcept
{
    return 'a' <= file && file <= 'h';
}

bool Square::inside_board(const int rank) noexcept
{
    return 1 <= rank && rank <= 8;
}

bool Square::inside_board(const char file, const int rank) noexcept
{
    return inside_board(file) && inside_board(rank);
}

All_Squares Square::all_squares() noexcept
{
    return {};
}

Squares_in_a_Line Square::squares_between(const Square a, const Square b) noexcept
{
    return Squares_in_a_Line(a, b);
}

Squares_in_a_Line Square::square_line_from(const Square origin, const Square_Difference& direction) noexcept
{
    return Squares_in_a_Line(origin, direction);
}

bool operator==(const Square a, const Square b) noexcept
{
    return (a.index() == b.index()) || ( ! a.inside_board() && ! b.inside_board());
}

bool operator!=(const Square a, const Square b) noexcept
{
    return !(a == b);
}

Square operator+(Square square, const Square_Difference& diff) noexcept
{
    return square += diff;
}

Square operator-(Square square, const Square_Difference& diff) noexcept
{
    return square -= diff;
}

Square_Difference operator-(const Square a, const Square b) noexcept
{
    assert(a.inside_board() && b.inside_board());
    return {a.file() - b.file(), a.rank() - b.rank()};
}

bool straight_line_move(const Square start, const Square end) noexcept
{
    const auto move = end - start;
    return move.file_change == 0 ||
           move.rank_change == 0 ||
           std::abs(move.file_change) == std::abs(move.rank_change);
}

bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2) noexcept
{
    // Think of the determinant of a 2x2 matrix with the two moves as column vectors.
    // Parallel (including anti-parallel) vectors are not linearly independent, so
    // the determinant of the matrix is zero.
    return move_1.file_change*move_2.rank_change == move_2.file_change*move_1.rank_change;
}

bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2) noexcept
{
    return moves_are_parallel(move_1, move_2) &&
           move_1.file_change*move_2.file_change + move_1.rank_change*move_2.rank_change >= 0; // dot product
}

bool in_line_in_order(const Square a, const Square b, const Square c) noexcept
{
    return same_direction(a - b, b - c);
}

Square_Line_Iterator::Square_Line_Iterator(const Square start, const Square_Difference& square_step) noexcept :
    current_square(start),
    step(square_step)
{
}

Square_Line_Iterator& Square_Line_Iterator::operator++() noexcept
{
    current_square += step;
    return *this;
}

bool Square_Line_Iterator::operator==(const Square_Line_Iterator& other) const noexcept
{
    return current_square == other.current_square;
}

bool Square_Line_Iterator::operator!=(const Square_Line_Iterator& other) const noexcept
{
    return ! (*this == other);
}

Square Square_Line_Iterator::operator*() const noexcept
{
    return current_square;
}

Squares_in_a_Line::Squares_in_a_Line(const Square first_border, const Square second_border) noexcept :
    border1(first_border),
    border2(second_border),
    step((second_border - first_border).step())
{
}

Squares_in_a_Line::Squares_in_a_Line(const Square origin, const Square_Difference& square_step) noexcept :
    border1(origin),
    border2{},
    step(square_step)
{
}

Square_Line_Iterator Squares_in_a_Line::begin() const noexcept
{
    return Square_Line_Iterator(border1 + step, step);
}

Square_Line_Iterator Squares_in_a_Line::end() const noexcept
{
    return {border2, step};
}

All_Squares_Iterator::All_Squares_Iterator(const Square start) noexcept : current_square(start)
{
}

All_Squares_Iterator& All_Squares_Iterator::operator++() noexcept
{
    ++current_square;
    return *this;
}

bool All_Squares_Iterator::operator==(const All_Squares_Iterator& other) const noexcept
{
    return current_square == other.current_square;
}

bool All_Squares_Iterator::operator!=(const All_Squares_Iterator& other) const noexcept
{
    return ! (*this == other);
}

Square All_Squares_Iterator::operator*() const noexcept
{
    return current_square;
}
