#include "Game/Square.h"

#include <cstddef>
#include <string>
#include <cassert>
#include <cmath>

#include "Game/Color.h"

#include "Utility/Math.h"

Square_Difference::Square_Difference(const int file_change, const int rank_change) noexcept : index_delta(rank_change + Square::board_representation_height()*file_change)
{
}

Square_Difference::Square_Difference(const int index_change) noexcept : index_delta(index_change)
{
}

Square_Difference Square_Difference::operator-() const noexcept
{
    return Square_Difference{-index_change()};
}

Square_Difference Square_Difference::step() const noexcept
{
    const auto abs_change = std::abs(index_change());
    if(abs_change <= 7)
    {
        return {Math::sign(index_change())}; // vertical move
    }
    else if(abs_change % Square::board_representation_height() == 0)
    {
        return {Math::sign(index_change())*int(Square::board_representation_height())}; // horizontal move
    }
    else if(abs_change % (Square::board_representation_height() + 1) == 0)
    {
        return {Math::sign(index_change())*int(Square::board_representation_height() + 1)}; // diagonal up-left/down-right
    }
    else if(abs_change % (Square::board_representation_height() - 1) == 0)
    {
        return {Math::sign(index_change())*int(Square::board_representation_height() - 1)}; // diagonal up-right/down-left
    }
    else
    {
        return *this; // knight move
    }
}

All_Squares_Iterator All_Squares::begin() const noexcept
{
    return All_Squares_Iterator({'a', 1});
}

All_Squares_Iterator All_Squares::end() const noexcept
{
    return All_Squares_Iterator({});
}

std::string Square::text() const noexcept
{
    return is_set() ? (file() + std::to_string(rank())) : "-";
}

Square_Color Square::color() const noexcept
{
    return (index()/BOARD_HEIGHT)%2 == (index()%BOARD_WIDTH)%2 ? Square_Color::WHITE : Square_Color::BLACK;
}

Square& Square::operator+=(const Square_Difference& diff) noexcept
{
    square_index += diff.index_change();
    return *this;
}

Square& Square::operator-=(const Square_Difference& diff) noexcept
{
    return *this += -diff;
}

Square& Square::operator++() noexcept
{
    constexpr auto max_index = Square('h', 8).index();
    do
    {
        ++square_index;
    } while( ! inside_board() && square_index <= max_index);

    return *this;
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
    return {int(a.index()) - int(b.index())};
}

bool straight_line_move(const Square start, const Square end) noexcept
{
    const auto move = std::abs((end - start).index_change());
    return move % Square::board_representation_height() == 0 ||
           move < int(Square::board_representation_height()) ||
           move % (Square::board_representation_height() + 1) == 0 ||
           move % (Square::board_representation_height() - 1) == 0;
}

bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2) noexcept
{
    return std::abs(move_1.step().index_change()) == std::abs(move_2.step().index_change());
}

bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2) noexcept
{
    return moves_are_parallel(move_1, move_2)
        && move_1.index_change()*move_2.index_change() > 0; // same sign
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
