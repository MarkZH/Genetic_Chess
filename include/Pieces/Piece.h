#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>

#include "Moves/Move.h"
#include "Game/Color.h"
#include "Game/Board.h"

class Piece
{
    public:
        Piece(Color color_in, const std::string& symbol_in);
        virtual ~Piece() = 0;

        virtual std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(unsigned int row) const;
        Color color() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& get_move_list(char file, int rank) const;

        bool operator==(const Piece& other) const;
        bool operator!=(const Piece& other) const;

        virtual bool is_pawn() const;
        virtual bool is_king() const;
        virtual bool is_queen() const;
        virtual bool is_rook() const;
        virtual bool is_bishop() const;
        virtual bool is_knight() const;

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

        // Contains pointers to move data
        std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::vector<std::vector<const Move*>> legal_moves;

        void add_legal_move(const Move* move);
};

#endif // PIECE_H
