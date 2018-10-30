#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Game/Color.h"
#include "Piece_Types.h"

class Move;

class Piece
{
    public:
        Piece(Color color_in, Piece_Type type_in);
        virtual ~Piece() = 0;

        std::string pgn_symbol() const;
        char fen_symbol() const;
        Color color() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& move_list(char file, int rank) const;

        Piece_Type type() const;

    protected:
        // Add a move to the list that is only legal when starting from a certain square
        // (e.g., castling, pawn double move, promotion, etc.)
        void add_legal_move(std::unique_ptr<Move> move);

        // Add a move to the list that is legal starting from all squares
        void add_standard_legal_move(int file_step, int rank_step);

    private:
        Color my_color;
        Piece_Type my_type;

        // Contains pointers to move data
        std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::array<std::vector<const Move*>, 64> legal_moves;
};

#endif // PIECE_H
