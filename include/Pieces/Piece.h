#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>

#include "Moves/Move.h"
#include "Game/Color.h"

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

        const std::vector<std::unique_ptr<const Move>>& get_move_list() const;

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
        std::vector<std::unique_ptr<const Move>> possible_moves;

    private:
        Color my_color;
        std::string symbol;
};

#endif // PIECE_H
