#include "Game/Square.h"

#include <cstddef>
#include <string>
#include <cassert>
#include <cmath>
#include <array>
#include <cassert>

#include "Game/Color.h"

#include "Utility/Math.h"

namespace
{
    using square_index_t = Square::square_index_t;

    const square_index_t GAME_BOARD_LENGTH = 8;
    const square_index_t BOARD_WIDTH_MARGIN = 1;
    const square_index_t BOARD_WIDTH = GAME_BOARD_LENGTH + 2 * BOARD_WIDTH_MARGIN;
    const square_index_t BOARD_HEIGHT_MARGIN = 2;
    const square_index_t BOARD_HEIGHT = GAME_BOARD_LENGTH + 2 * BOARD_HEIGHT_MARGIN;
    const square_index_t BOARD_SIZE = BOARD_HEIGHT * BOARD_WIDTH;

    square_index_t index_to_row(square_index_t index) noexcept
    {
        return index % BOARD_HEIGHT;
    }

    square_index_t index_to_col(square_index_t index) noexcept
    {
        return index / BOARD_HEIGHT;
    }

    bool is_valid_row(square_index_t row) noexcept
    {
        return row >= BOARD_HEIGHT_MARGIN && row < BOARD_HEIGHT_MARGIN + GAME_BOARD_LENGTH;
    }

    bool is_valid_col(square_index_t col) noexcept
    {
        return col >= BOARD_WIDTH_MARGIN && col < BOARD_WIDTH_MARGIN + GAME_BOARD_LENGTH;
    }

    bool index_on_game_board(square_index_t index) noexcept
    {
        return is_valid_col(index_to_col(index)) && is_valid_row(index_to_row(index));
    }

    square_index_t board_index_to_game_index(square_index_t board_index) noexcept
    {
        const auto row = index_to_row(board_index);
        const auto col = index_to_col(board_index);
        return (row - BOARD_HEIGHT_MARGIN) + GAME_BOARD_LENGTH * (col - BOARD_WIDTH_MARGIN);
    }

    const std::array<square_index_t, BOARD_SIZE> indices = []()
        {
            auto index_array = std::array<square_index_t, BOARD_SIZE>{};
            const auto invalid_index = square_index_t(-1);
            for(square_index_t i = 0; i < BOARD_SIZE; ++i)
            {
                if(index_on_game_board(i))
                {
                    index_array[i] = board_index_to_game_index(i);
                }
                else
                {
                    index_array[i] = invalid_index;
                }
            }

            return index_array;
        }();

    const std::array<char, BOARD_SIZE> files = []()
        {
            auto file_array = std::array<char, BOARD_SIZE>{};
            const auto invalid_file = '\0';
            for(square_index_t i = 0; i < BOARD_SIZE; ++i)
            {
                if(index_on_game_board(i))
                {
                    file_array[i] = char(index_to_col(i) - BOARD_WIDTH_MARGIN) + 'a';
                }
                else
                {
                    file_array[i] = invalid_file;
                }
            }

            return file_array;
        }();

    const std::array<int, BOARD_SIZE> ranks = []()
        {
            auto rank_array = std::array<int, BOARD_SIZE>{};
            const auto invalid_rank = -1;
            for(square_index_t i = 0; i < BOARD_SIZE; ++i)
            {
                if(index_on_game_board(i))
                {
                    rank_array[i] = int(index_to_row(i) - BOARD_HEIGHT_MARGIN) + 1;
                }
                else
                {
                    rank_array[i] = invalid_rank;
                }
            }

            return rank_array;
        }();
}

int Square_Difference::index_change() const noexcept
{
    return BOARD_HEIGHT*file_change + rank_change;
}

Square_Difference Square_Difference::operator-() const noexcept
{
    return {-file_change, -rank_change};
}

Square_Difference Square_Difference::step() const noexcept
{
    if(straight_line_move(*this))
    {
        return {Math::sign(file_change), Math::sign(rank_change)};
    }
    else
    {
        return *this;
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

Square::Square() noexcept : square_index(BOARD_SIZE)
{
}

Square::Square(const char file, const int rank) noexcept : square_index(BOARD_HEIGHT*(file - 'a' + BOARD_WIDTH_MARGIN) + (rank - 1 + BOARD_HEIGHT_MARGIN))
{
}

char Square::file() const noexcept
{
    assert(inside_board());
    return files[square_index];
}

int Square::rank() const noexcept
{
    assert(inside_board());
    return ranks[square_index];
}

Square::square_index_t Square::index() const noexcept
{
    return indices[square_index];
}

std::string Square::text() const noexcept
{
    return is_set() ? (file() + std::to_string(rank())) : "-";
}

Square_Color Square::color() const noexcept
{
    return (square_index/BOARD_HEIGHT)%2 == (square_index%BOARD_WIDTH)%2 ? Square_Color::WHITE : Square_Color::BLACK;
}

bool Square::inside_board() const noexcept
{
    return square_index < BOARD_SIZE && indices[square_index] < 64;
}

bool Square::is_set() const noexcept
{
    return inside_board();
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
    do
    {
        ++square_index;
    } while( ! inside_board() && square_index < BOARD_SIZE);

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
    return (a.square_index == b.square_index) || ( ! a.inside_board() && ! b.inside_board());
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
    return straight_line_move(end - start);
}

bool straight_line_move(const Square_Difference& diff) noexcept
{
    return diff.file_change == 0 || diff.rank_change == 0 || std::abs(diff.file_change) == std::abs(diff.rank_change);
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
