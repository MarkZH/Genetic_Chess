#ifndef SQUARE_H
#define SQUARE_H

#include <cstddef>
#include <string>

#include "Game/Color.h"

//! \file

//! A container for offsets between squares.
struct Square_Difference
{
    int file_change;
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
        Square& operator++();
        Square operator*() const;

        static bool inside_board(char file);
        static bool inside_board(int  rank);
        static bool inside_board(char file, int  rank);
        bool inside_board() const;
        bool is_set() const;

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

struct Square_Iterator
{
    Square begin() const;
    Square end() const;
};

#endif // SQUARE_H
