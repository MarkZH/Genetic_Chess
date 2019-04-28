#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>

#include "Game/Color.h"
#include "Piece_Types.h"

class Move;
class Square;

//! A class to represent chess pieces.

//! Possible moves are stored within each piece.
class Piece
{
    public:
        Piece();
        Piece(Color color, Piece_Type type);

        std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(size_t row, size_t square_height) const;

        Color color() const;
        Piece_Type type() const;
        operator bool() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& move_list(Square square) const;
        const std::vector<const Move*>& attacking_moves(Square square) const;

    private:
        unsigned int piece_code;
        static const unsigned int invalid_code;

        friend bool operator==(Piece a, Piece b);
};

bool operator==(Piece a, Piece b);
bool operator!=(Piece a, Piece b);

#endif // PIECE_H
