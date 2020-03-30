#ifndef PIECE_H
#define PIECE_H

#include <string>

#include "Utility/Fixed_Capacity_Vector.h"

#include "Game/Color.h"

class Move;
class Square;

//! \file

//! \brief An enumeration to identify the type of a Piece.
enum class Piece_Type
{
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

//! \brief A class to represent chess pieces.
//!
//! Possible moves are stored within each piece.
class Piece
{
    private:
        using piece_code_t = unsigned int;
        using list_of_move_lists = Fixed_Capacity_Vector<Fixed_Capacity_Vector<const Move*, 7>, 12>;

    public:
        //! \brief Create an invalid piece that can represent an unoccupied space on a Board.
        Piece() noexcept;

        //! \brief Create a piece.
        //!
        //! \param color The color of the piece.
        //! \param type The type of piece.
        Piece(Piece_Color color, Piece_Type type) noexcept;

        //! \brief Create a piece from a single character PGN symbol
        //!
        //! \param pgn_symbol A character indicating a piece in PGN format (uppercase for white,
        //!        lowercase for black, and the letter indicating the piece type).
        explicit Piece(char pgn_symbol);

        //! \brief Get the PGN symbol for the piece.
        //!
        //! \returns The symbol for the moving piece when writing a game record. A pawn is represented by an empty string.
        std::string pgn_symbol() const noexcept;

        // \brief Get the piece symbol when writing an FEN string.
        //!
        //! \returns A single character symbol for the piece. Uppercase is white, lowercase is black.
        char fen_symbol() const noexcept;

        //! \brief The color of the piece.
        //!
        //! \returns The Piece_Color of the player that controls the piece.
        Piece_Color color() const noexcept;

        //! \brief Get the type of the piece.
        //!
        //! \returns The kind of piece, i.e., PAWN, ROOK, etc.
        Piece_Type type() const noexcept;

        //! \brief Returns true if the piece is valid. An invalid piece represents an empty square on a Board.
        operator bool() const noexcept;

        //! \brief Returns an unsigned integer useful for indexing arrays.
        piece_code_t index() const noexcept;

        //! \brief Check that a piece is allowed to make a certain move.
        //!
        //! \param move A pointer to a prospective move.
        //! \returns Whether or not the piece is allowed to move in the manner described by the parameter.
        bool can_move(const Move* move) const noexcept;

        //! \brief Get all possibly legal moves of a piece starting from a given square.
        //!
        //! \param square The square where the moves start.
        //! \returns A list of lists of legal moves starting from that square. The moves are grouped into
        //!          lists by direction and ordered by distance from the starting square.
        const list_of_move_lists& move_lists(Square square) const noexcept;

        //! \brief Gives all moves that are allowed to capture other pieces.
        //!
        //! \param square The square where the attacking moves start.
        //! \returns A list of lists of moves grouped and ordered as in Piece::move_lists().
        const list_of_move_lists& attacking_move_lists(Square square) const noexcept;

    private:
        piece_code_t piece_code;
        static const piece_code_t invalid_code;
};

//! \brief Check two pieces for equality
//!
//! \param a A piece.
//! \param b Another piece.
//! \returns Whether the two pieces are the same type and color
bool operator==(Piece a, Piece b) noexcept;

//! \brief Check two pieces for inequality
//!
//! \param a A piece.
//! \param b Another piece.
//! \returns Whether the two pieces have different types or colors.
bool operator!=(Piece a, Piece b) noexcept;

#endif // PIECE_H
