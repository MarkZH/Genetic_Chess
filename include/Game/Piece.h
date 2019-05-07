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
        Piece();
        Piece(Color color, Piece_Type type);

        std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(size_t row, size_t square_height) const;

        Color color() const;
        Piece_Type type() const;
        operator bool() const;
        piece_code_t index() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& move_list(Square square) const;
        const std::vector<const Move*>& attacking_moves(Square square) const;

    private:
        piece_code_t piece_code;
        static const piece_code_t invalid_code;
};

bool operator==(Piece a, Piece b);
bool operator!=(Piece a, Piece b);

#endif // PIECE_H
