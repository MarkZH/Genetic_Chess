#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Game/Board.h"
#include "Game/Color.h"
#include "Piece_Types.h"

class Move;

//! A class to represent chess pieces.

//! Possible moves are stored within each piece.
class Piece
{
    public:
        Piece(Color color_in, Piece_Type type_in);

        std::string pgn_symbol() const;
        char fen_symbol() const;
        std::string ascii_art(size_t row, size_t square_height) const;
        Color color() const;

        bool can_move(const Move* move) const;

        const std::vector<const Move*>& move_list(Square square) const;
        const std::vector<const Move*>& attacking_moves(Square square) const;

        Piece_Type type() const;

    private:
        Color my_color;
        Piece_Type my_type;

        // Contains pointers to move data
        std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::array<std::vector<const Move*>, 64> legal_moves;

        // Holds list of all moves that can attack another square
        std::array<std::vector<const Move*>, 64> attack_moves;

        void add_pawn_moves();
        void add_rook_moves();
        void add_knight_moves();
        void add_bishop_moves();
        void add_king_moves();

        // Add a move to the list that is only legal when starting from a certain square
        // (e.g., castling, pawn double move, promotion, etc.)
        template<typename Move_Type, typename ...Parameters>
        void add_legal_move(Parameters ... parameters)
        {
            auto move = std::make_unique<Move_Type>(parameters...);
            auto index = move->start().index();
            legal_moves[index].push_back(move.get());

            // Make list of all capturing moves, excluding all but one type of pawn capture per square.
            if(move->can_capture()
               && ! move->is_en_passant()
               && (move->promotion_piece_symbol() == 'Q' || move->promotion_piece_symbol() == '\0'))
            {
                attack_moves[index].push_back(move.get());
            }
            possible_moves.push_back(std::move(move));
        }

        // Add a move to the list that is legal starting from all squares
        void add_standard_legal_move(int file_step, int rank_step);

        std::vector<std::string> ascii_art_lines;
        static size_t maximum_piece_height;

        void add_pawn_art();
        void add_rook_art();
        void add_knight_art();
        void add_bishop_art();
        void add_queen_art();
        void add_king_art();
        void add_color();
};

#endif // PIECE_H
