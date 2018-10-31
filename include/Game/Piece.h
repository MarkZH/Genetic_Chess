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

        std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(size_t row) const;
        Color color() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& move_list(char file, int rank) const;

        Piece_Type type() const;

    private:
        Color my_color;
        Piece_Type my_type;

        // Contains pointers to move data
        std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::array<std::vector<const Move*>, 64> legal_moves;

        void add_pawn_moves();
        void add_rook_moves();
        void add_knight_moves();
        void add_bishop_moves();
        void add_king_moves();

        // Add a move to the list that is only legal when starting from a certain square
        // (e.g., castling, pawn double move, promotion, etc.)
        void add_legal_move(std::unique_ptr<Move> move);

        // Add a move to the list that is legal starting from all squares
        void add_standard_legal_move(int file_step, int rank_step);

        std::vector<std::string> ascii_art_lines;

        void add_pawn_art();
        void add_rook_art();
        void add_knight_art();
        void add_bishop_art();
        void add_queen_art();
        void add_king_art();
};

#endif // PIECE_H
