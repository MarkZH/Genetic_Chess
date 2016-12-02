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
        explicit Piece(Color color_in);
        Piece(const Piece&) = delete;
        Piece& operator=(const Piece&) = delete;
        virtual ~Piece();

        virtual std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(unsigned int row) const;
        Color color() const;
        std::vector<std::shared_ptr<const Move>> get_legal_moves(const Board& board,
                                                                 char file_start, int rank_start,
                                                                 char file_end,   int rank_end,
                                                                 bool king_check,
                                                                 char promote = '\0') const;

        bool can_move(const Move* move) const;

        const std::vector<std::shared_ptr<const Move>> get_move_list() const;

        bool operator==(const Piece& other) const;
        bool operator!=(const Piece& other) const;

    protected:
        Color my_color;
        std::string symbol;

        std::vector<std::shared_ptr<const Move>> possible_moves;
        std::vector<std::string> ascii_art_lines;
};

#endif // PIECE_H
