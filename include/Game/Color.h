#ifndef COLOR_H
#define COLOR_H

#include <string>

//! \file

//! \brief Used for identifying the color of a piece and the player controlling those pieces.
enum class Piece_Color : unsigned
{
    WHITE,
    BLACK
};

//! \brief Returns the opposite of the input color: Piece_Color::BLACK to Piece_Color::WHITE or Piece_Color::WHITE to Piece_Color::BLACK.
//!
//! \param color The color to reverse.
//! \returns Opposite color.
Piece_Color opposite(Piece_Color color) noexcept;

//! \brief Returns a text (std::string) version of a color.
//!
//! \param color The color to convert to text.
//! \returns A textual representation of the color.
std::string color_text(Piece_Color color) noexcept;

//! \brief A representation of the color of Squares on a Board.
enum class Square_Color
{
    WHITE,
    BLACK
};

//! \brief Returns the opposite of a given Square_Color.
//!
//! \param color The color whose opposite is sought.
Square_Color opposite(Square_Color color) noexcept;

//! \brief A representation of the winner of a game.
enum class Winner_Color : unsigned
{
    WHITE,
    BLACK,
    NONE
};

//! Returns the text representation of a Winner_Color.
//!
//! \param color The given Square_Color.
std::string color_text(Winner_Color color) noexcept;

#endif // COLOR_H
