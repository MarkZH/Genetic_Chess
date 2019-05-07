#ifndef SQUARE_H
#define SQUARE_H

#include <cstddef>
#include <string>

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

struct Square_Iterator;

//! This class represents a location on the board.
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
        Square& operator++();
        Square operator*() const;
        static Square_Iterator all_squares();

    private:
        square_index_t square_index;
        static const square_index_t invalid_index;
};

bool operator==(Square a, Square b);
bool operator!=(Square a, Square b);

Square operator+(Square s, const Square_Difference& diff);
Square operator-(Square s, const Square_Difference& diff);
Square_Difference operator-(Square a, Square b);

bool straight_line_move(Square start, Square rank);
bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2);
bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2);

//! A pseudo-container for iterating over all squares of a Board.
//
//! The iteration happens in an unspecified order to allow
//! for optimization.
struct Square_Iterator
{
    Square begin() const;
    Square end() const;
};

#endif // SQUARE_H
