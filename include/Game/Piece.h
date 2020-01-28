#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>

#include "Game/Color.h"

class Move;
class Square;

//! An enumeration to identify the type of a Piece.
enum Piece_Type : unsigned
{
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

//! A class to represent chess pieces.
//
//! Possible moves are stored within each piece.
class Piece
{
    private:
        using piece_code_t = unsigned int;

    public:
        //! Create an invalid piece that can represent an unoccupied space on a Board.
        Piece() noexcept;

        //! Create a piece.
        //
        //! \param color The color of the piece.
        //! \param type The type of piece.
        Piece(Color color, Piece_Type type) noexcept;

        //! Get the PGN symbol for the piece.
        //
        //! \returns The symbol for the moving piece when writing a game record. A pawn is represented by an empty string.
        std::string pgn_symbol() const noexcept;

        // Get the piece symbol when writing an FEN string.
        //
        //! \returns A single character symbol for the piece. Uppercase is white, lowercase is black.
        char fen_symbol() const noexcept;

        //! The color of the piece.
        //
        //! \returns The Color of the player that controls the piece.
        Color color() const noexcept;

        //! Get the type of the piece.
        //
        //! \returns The kind of piece, i.e., PAWN, ROOK, etc.
        Piece_Type type() const noexcept;

        //! Returns true if the piece is valid. An invalid piece represents an empty square on a Board.
        operator bool() const noexcept;

        //! Returns an unsigned integer useful for indexing arrays.
        piece_code_t index() const noexcept;

        //! Check that a piece is allowed to make a certain move.
        //
        //! \param move A pointer to a prospective move.
        //! \returns Whether or not the piece is allowed to move in the manner described by the parameter.
        bool can_move(const Move* move) const noexcept;

        //! Get all possibly legal moves of a piece starting from a given square.
        //
        //! \param square The square where the moves start.
        //! \returns A list of lists of legal moves starting from that square. The moves are grouped into
        //!          lists by direction and ordered by distance from the starting square.
        const std::vector<std::vector<const Move*>>& move_lists(Square square) const noexcept;

        //! Gives all moves that are allowed to capture other pieces.
        //
        //! \param square The square where the attacking moves start.
        //! \returns A list of lists of moves grouped and ordered as in Piece::move_lists().
        const std::vector<std::vector<const Move*>>& attacking_move_lists(Square square) const noexcept;

    private:
        piece_code_t piece_code;
        static const piece_code_t invalid_code;
};

//! Check two pieces for equality
//
//! \param a A piece.
//! \param b Another piece.
//! \returns Whether the two pieces are the same type and color
bool operator==(Piece a, Piece b) noexcept;

//! Check two pieces for inequality
//
//! \param a A piece.
//! \param b Another piece.
//! \returns Whether the two pieces have different types or colors.
bool operator!=(Piece a, Piece b) noexcept;

#endif // PIECE_H
