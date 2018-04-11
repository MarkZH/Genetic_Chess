#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>

#include "Moves/Move.h"
#include "Game/Color.h"
#include "Piece_Types.h"
#include "Game/Board.h"


class Piece
{
    public:
        Piece(Color color_in, const std::string& symbol_in, Piece_Type type_in);
        virtual ~Piece() = 0;

        virtual std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(size_t row) const;
        Color color() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& get_move_list(char file, int rank) const;
        std::array<bool, 64> all_attacked_squares(char file, int rank, const Board& board) const;

        Piece_Type type() const;

    protected:
        std::vector<std::string> ascii_art_lines;

        // Add a move to the list that is only legal when starting from a certain square
        // (e.g., castling, pawn double move, promotion, etc.)
        void add_legal_move(std::unique_ptr<Move> move);

        // Add a move to the list that is legal starting from all squares
        void add_standard_legal_move(int file_step, int rank_step);

    private:
        Color my_color;
        std::string symbol;
        Piece_Type my_type;

        // Contains pointers to move data
        std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::vector<std::vector<const Move*>> legal_moves;
};

#endif // PIECE_H
