#ifndef SQUARE_H
#define SQUARE_H

#include <cstddef>
#include <string>
#include <iterator>

#include "Game/Color.h"

//! \file

//! A container for offsets between squares.
struct Square_Difference
{
    //! The horizontal distance between two squares.
    int file_change;

    //! The vertical distance between two squares.
    int rank_change;

    Square_Difference operator-() const;
    Square_Difference step() const;
};

struct All_Squares;
class Squares_in_a_Line;
class All_Squares_Iterator;
class Square_Line_Iterator;

//! This class represents a location on the board.
//
//! It does not represent a physical square on which a Piece is placed.
//! It is more like a coordinate specifying a location.
class Square
{
    private:
        using square_index_t = unsigned int;

    public:
        Square();
        Square(char file, int rank);

        char file() const;
        int rank() const;
        square_index_t index() const;
        std::string string() const;
        Color color() const;

        Square& operator+=(const Square_Difference& diff);
        Square& operator-=(const Square_Difference& diff);

        static bool inside_board(char file);
        static bool inside_board(int  rank);
        static bool inside_board(char file, int  rank);
        bool inside_board() const;
        bool is_set() const;

        // Iterating methods
        static All_Squares all_squares();
        static Squares_in_a_Line squares_between(Square a, Square b);
        static Squares_in_a_Line square_line_from(Square origin, Square_Difference direction);

    private:
        square_index_t square_index;
        static const square_index_t invalid_index;

        Square& operator++();

        friend class All_Squares_Iterator;
        friend class Square_Line_Iterator;
};

bool operator==(Square a, Square b);
bool operator!=(Square a, Square b);

Square operator+(Square s, const Square_Difference& diff);
Square operator-(Square s, const Square_Difference& diff);
Square_Difference operator-(Square a, Square b);

bool straight_line_move(Square start, Square rank);
bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2);
bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2);
bool in_line_in_order(Square a, Square b, Square c);

//! The iterator created by the All_Squares container that dereferences to a Square.
class All_Squares_Iterator
{
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Square;
        using difference_type = int;
        using pointer = Square*;
        using reference = Square&;

        explicit All_Squares_Iterator(Square start);
        All_Squares_Iterator& operator++();
        bool operator==(const All_Squares_Iterator& other) const;
        bool operator!=(const All_Squares_Iterator& other) const;
        Square operator*() const;

    private:
        Square current_square;
};

//! A pseudo-container for iterating over all squares of a Board.
//
//! The iteration happens in an unspecified order to allow
//! for optimization.
struct All_Squares
{
    All_Squares_Iterator begin() const;
    All_Squares_Iterator end() const;
};

//! The iterator created by the Squares_in_a_Line container that dereferences to a Square.
class Square_Line_Iterator
{
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Square;
        using difference_type = int;
        using pointer = Square*;
        using reference = Square&;

        Square_Line_Iterator(Square start, const Square_Difference& square_step);
        Square_Line_Iterator& operator++();
        bool operator==(const Square_Line_Iterator& other) const;
        bool operator!=(const Square_Line_Iterator& other) const;
        Square operator*() const;

    private:
        Square current_square;
        Square_Difference step;
};

//! A pseudo-container that represesnts a set of squares along a line on the Board.
class Squares_in_a_Line
{
    public:
        Squares_in_a_Line(Square first_border, Square second_border);
        Squares_in_a_Line(Square origin, const Square_Difference& step);
        Square_Line_Iterator begin() const;
        Square_Line_Iterator end() const;

    private:
        Square border1;
        Square border2;
        Square_Difference step;
};

#endif // SQUARE_H
