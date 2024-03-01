#ifndef SQUARE_H
#define SQUARE_H

#include <cstddef>
#include <string>
#include <iterator>
#include <cassert>

#include "Game/Color.h"

//! \file

class Square;

//! \brief A container for offsets between squares.
class Square_Difference
{
    public:
        Square_Difference(int file_change, int rank_change) noexcept;

        int index_change() const noexcept
        {
            return index_delta;
        }

        //! \brief Reverse the direction of a square offset.
        Square_Difference operator-() const noexcept;

        //! \brief Returns a single-step version of the offset--i.e., (-3, 3) --> (-1, 1)
        //!
        //! Note: Only valid for straight (rook- or bishop-type) moves
        Square_Difference step() const noexcept;

    private:
        int index_delta;

        Square_Difference(int index_delta) noexcept;

        friend Square_Difference operator-(Square a, Square b) noexcept;
};

struct All_Squares;
class Squares_in_a_Line;
class All_Squares_Iterator;
class Square_Line_Iterator;

//! \brief This class represents a location on the board.
//!
//! It does not represent a physical square on which a Piece is placed.
//! It is more like a coordinate specifying a location.
class Square
{
    private:
        using square_index_t = unsigned int;

        static const square_index_t GAME_BOARD_SIZE = 8;
        static const square_index_t BOARD_WIDTH_MARGIN = 1;
        static const square_index_t BOARD_WIDTH = GAME_BOARD_SIZE + 2*BOARD_WIDTH_MARGIN;
        static const square_index_t BOARD_HEIGHT_MARGIN = 2;
        static const square_index_t BOARD_HEIGHT = GAME_BOARD_SIZE + 2*BOARD_HEIGHT_MARGIN;

    public:
        //! \brief The default constructor creates an invalid square location.
        constexpr Square() noexcept : square_index(board_representation_size())
        {
        }

        //! \brief This constructor creates a user-defined square.
        //!
        //! \param file The file (column) of the square (a-h).
        //! \param rank The rank (row) of the square (1-8).
        //!
        //! The validity of the square coordinates is not checked in release builds.
        //! In debug builds, invalid square coordinates (e.g., "i9") trigger an assertion failure.
        constexpr Square(char file, int rank) noexcept : square_index(BOARD_HEIGHT*(file - 'a' + BOARD_WIDTH_MARGIN) + (rank - 1 + BOARD_HEIGHT_MARGIN))
        {
        }

        //! \brief The file of the square.
        //!
        //! \returns The letter label of the square file.
        constexpr char file() const noexcept
        {
            return char('a' + index()/BOARD_HEIGHT - BOARD_WIDTH_MARGIN);
        }

        //! \brief The rank of the square.
        //!
        //! \returns The numerical label of the rank.
        constexpr int rank() const noexcept
        {
            return 1 + index()%BOARD_HEIGHT - BOARD_HEIGHT_MARGIN;
        }

        //! \brief The index of the square.
        //!
        //! \returns An unsigned integer index.
        constexpr square_index_t index() const noexcept
        {
            return square_index;
        }

        constexpr square_index_t index64() const noexcept
        {
            return (rank() - 1) + (file() - 'a')*GAME_BOARD_SIZE;
        }

        //! \brief Text representation of square.
        std::string text() const noexcept;

        //! \brief Returns the color of the square.
        //!
        //! This function is useful for Board::enough_material_to_checkmate() since one
        //! condition is that bishops on opposite color squares are sufficient to checkmate,
        //! while bishops on the same color square are not.
        //! \returns Square_Color of square on board ("a1" --> Square_Color::BLACK; "h1" --> Square_Color::WHITE)
        Square_Color color() const noexcept;

        //! \brief Apply an offset to a square.
        //!
        //! \param diff A pair of integers representing the change in file and rank
        //! \returns A reference to the now-modified square. This may create a square that is off
        //!          the board ("h8" + (1,1), for example).
        Square& operator+=(const Square_Difference& diff) noexcept;

        //! \brief Apply an offset to a square in the opposite direction.
        //!
        //! \param diff A pair of integers representing the change in file and rank
        //! \returns A reference to the now-modified square.
        Square& operator-=(const Square_Difference& diff) noexcept;

        //! \brief Check if the square is a valid Board position.
        //!
        //! \returns Whether the square is on the Board ("i10" returns false).
        constexpr bool inside_board() const noexcept
        {
            const auto ind = index();
            const auto row = ind % BOARD_HEIGHT;
            const auto col = ind / BOARD_HEIGHT;
            return row >= BOARD_HEIGHT_MARGIN && row < BOARD_HEIGHT_MARGIN + GAME_BOARD_SIZE
                && col >= BOARD_WIDTH_MARGIN && col < BOARD_WIDTH_MARGIN + GAME_BOARD_SIZE;
        }

        //! \brief Check whether a square has been set with a valid coordinate.
        //!
        //! This is a synonym for Square::inside_board().
        constexpr bool is_set() const noexcept
        {
            return inside_board();
        }

        static constexpr square_index_t board_representation_size() noexcept
        {
            return BOARD_WIDTH*BOARD_HEIGHT;
        }

        static constexpr square_index_t board_representation_height() noexcept
        {
            return BOARD_HEIGHT;
        }

        // Iterating methods

        //! \brief Returns a pseudo-container than can iterator over all square on a Board.
        //!
        //! The primary use of this method is to replace
        //! \code{cpp}
        //!     for(char file = 'a'; file <= 'h'; ++file)
        //!     {
        //!         for(int rank = 1; rank <= 8; ++rank)
        //!         {
        //!             auto square = Square{file, rank};
        //!             // ...
        //!         }
        //!     }
        //! \endcode
        //! with
        //! \code{cpp}
        //!     for(auto square : Square::all_squares())
        //!     {
        //!         // ...
        //!     }
        //! \endcode
        //!
        //! Use when the order of Square iteration doesn't matter.
        static All_Squares all_squares() noexcept;

        //! \brief Create a container of Squares between--but not including--two squares.
        //!
        //! \param a A square that borders the collection but is not included.
        //! \param b Another square that borders the collection but is not included.
        //! \returns A Squares_in_a_Line instance for use in a range-for loop or \<algorithms\>.
        //!
        //! This structure only works if the Squares in the arguments are along a common
        //! row, column, or diagonal.
        static Squares_in_a_Line squares_between(Square a, Square b) noexcept;

        //! \brief Create a container of Squares along a line specified by a Square and a direction.
        //!
        //! \param origin A square that starts a line of Squares but is not included in the collection.
        //! \param direction The direction that the line should proceed along. There is no restriction
        //!        on the direction or length of the difference. A knight move is valid.
        //!
        //! The end of the collection is when the squares leave the Board.
        static Squares_in_a_Line square_line_from(Square origin, const Square_Difference& direction) noexcept;

    private:
        square_index_t square_index;

        //! \brief Go to the next Square when iterating over Square::all_squares().
        Square& operator++() noexcept;

        friend class All_Squares_Iterator;
        friend class Square_Line_Iterator;
};

//! \brief Check if two squares are the same.
//!
//! \returns Whether two squares have the same file and rank or are both outside the board.
bool operator==(Square a, Square b) noexcept;

//! \brief Check if two squares are not the same.
//!
//! \returns Whether two squares differ in their file or rank.
bool operator!=(Square a, Square b) noexcept;


//! \brief Add an offset to a square, returning a new Square.
//!
//! \param square The original square.
//! \param diff An offset between two squares.
Square operator+(Square square, const Square_Difference& diff) noexcept;

//! \brief Add an offset to a square in the opposite direction, returning a new Square.
//!
//! \param square The original square.
//! \param diff An offset between two squares.
Square operator-(Square square, const Square_Difference& diff) noexcept;

//! \brief Find the 2-dimensional distance offset between two squares.
//!
//! \param a The first square.
//! \param b The second square.
//! \returns A pair of integers representing the signed difference in file and rank.
//!
//! For all valid squares on a board, a + (b - a) == b.
Square_Difference operator-(Square a, Square b) noexcept;


//! \brief Returns whether a rook or bishop could move from one square to another in one move.
//!
//! \param a A square on the board.
//! \param b Another square on the board.
bool straight_line_move(Square a, Square b) noexcept;

//! \brief Returns whether two Square_Differences are in the exact same or exact opposite directions.
//!
//! For example, (2, 2) is parallel to (3, 3) and (-1, -1)
//!
//! Note: A difference of (0, 0) is parallel with every other difference.
//!
//! \param move_1 A movement direction (length of the move does not matter).
//! \param move_2 A movement direction (length of the move does not matter).
bool moves_are_parallel(const Square_Difference& move_1, const Square_Difference& move_2) noexcept;

//! \brief Returns whether two Square_Differences are parallel in the same direction.
//!
//! For example, (2, 2) and (-1, -1) are parallel but not in the same direction,
//! whereas (2, 2) and (1, 1) are parallel and in the same direction.
//!
//! Note: A difference of (0, 0) is in the same direction as every other difference.
//!
//! \param move_1 A movement direction (length of the move does not matter).
//! \param move_2 A movement direction (length of the move does not matter).
bool same_direction(const Square_Difference& move_1, const Square_Difference& move_2) noexcept;

//! \brief Returns whether the three squares form a line in the order given.
//!
//! Note: Trivially true if a == b or b == c.
//!
//! \param a A square that should be to one side.
//! \param b Another square that should lie between the other two.
//! \param c A final square that should be on the opposite side of b to a.
bool in_line_in_order(Square a, Square b, Square c) noexcept;

//! \brief The iterator created by the All_Squares container that dereferences to a Square.
class All_Squares_Iterator
{
    public:
        //! \brief This iterator is meant to be traversed once.
        using iterator_category = std::input_iterator_tag;
        //! \brief The iterator dereferences to a Square.
        using value_type = Square;
        //! \brief The distance between iterators is an int.
        using difference_type = int;
        //! \brief The pointer to the data is a pointer to a Square.
        using pointer = Square*;
        //! \brief The reference to the data is reference to a Square.
        using reference = Square&;

        //! \brief Create an iterator referring to a square.
        //!
        //! \param start The square the iterator will refer to.
        explicit All_Squares_Iterator(Square start) noexcept;

        //! \brief Go to the next Square along the line.
        All_Squares_Iterator& operator++() noexcept;

        //! \brief Iterators are equal when they refer to the same Square.
        bool operator==(const All_Squares_Iterator& other) const noexcept;

        //! \brief Iterators are unequal when they refer to different Squares.
        bool operator!=(const All_Squares_Iterator& other) const noexcept;

        //! \brief Get the Square referred to by the iterator.
        Square operator*() const noexcept;

    private:
        Square current_square;
};

//! \brief A pseudo-container for iterating over all squares of a Board.
//!
//! The iteration happens in an unspecified order to allow
//! for optimization.
struct All_Squares
{
    //! \brief Return the first square iterator when iterating all squares.
    All_Squares_Iterator begin() const noexcept;

    //! \brief Return the end iterator for all squares.
    All_Squares_Iterator end() const noexcept;
};

//! \brief The iterator created by the Squares_in_a_Line container that dereferences to a Square.
class Square_Line_Iterator
{
    public:
        //! \brief This iterator is meant to be traversed once.
        using iterator_category = std::input_iterator_tag;
        //! \brief The iterator dereferences to a Square.
        using value_type = Square;
        //! \brief The distance between iterators is an int.
        using difference_type = int;
        //! \brief The pointer to the data is a pointer to a Square.
        using pointer = Square*;
        //! \brief The reference to the data is reference to a Square.
        using reference = Square&;

        //! \brief Construct the collection of in-line Squares.
        //!
        //! \param start The start of the line of squares (not included in the collection).
        //! \param square_step The direction of the line extending away from start.
        Square_Line_Iterator(Square start, const Square_Difference& square_step) noexcept;

        //! \brief Advance the iterator to the next Square in the line.
        Square_Line_Iterator& operator++() noexcept;

        //! \brief Iterators are equal if they refer to the same square.
        bool operator==(const Square_Line_Iterator& other) const noexcept;

        //! \brief Iterators are different if they refer to different squares.
        bool operator!=(const Square_Line_Iterator& other) const noexcept;

        //! \brief Get the Square referred to by the iterator.
        Square operator*() const noexcept;

    private:
        Square current_square;
        Square_Difference step;
};

//! \brief A pseudo-container that represesnts a set of squares along a line on the Board.
class Squares_in_a_Line
{
    public:
        //! \brief Create a collection of Squares between--but not including--the parameter Squares.
        //!
        //! \param first_border A Square that borders the collection.
        //! \param second_border Another Square that borders the collection.
        //!
        //! The parameter Squares must be along a common row, column, or diagonal.
        Squares_in_a_Line(Square first_border, Square second_border) noexcept;

        //! \brief Create a collections of Squares along a line starting at--but not including--a Square.
        //!
        //! \param origin The start of the line of Squares.
        //! \param step The direction the line proceeds. There are no restrictions on the
        //!        direction or step size.
        Squares_in_a_Line(Square origin, const Square_Difference& step) noexcept;

        //! \brief Start the Square-line collection.
        Square_Line_Iterator begin() const noexcept;

        //! \brief End the Square-line collection.
        Square_Line_Iterator end() const noexcept;

    private:
        Square border1;
        Square border2;
        Square_Difference step;
};

#endif // SQUARE_H
