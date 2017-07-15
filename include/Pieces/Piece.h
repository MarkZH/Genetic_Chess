#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <vector>
#include <memory>

#include "Moves/Move.h"
#include "Moves/Complete_Move.h"
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

        bool can_move(const Move* move, char file_start, int rank_start) const;

        const std::vector<Complete_Move>& get_move_list(char file, int rank) const;

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
        template<typename Move_Type, typename ...Move_Args>
        void add_special_legal_move(char file, int rank, Move_Args... args)
        {
            auto move = std::make_unique<Move_Type>(args...);
            add_legal_move(move.get(), file, rank);
            possible_moves.push_back(std::move(move));
        }


        // Add a move to the list that is legal starting from all squares
        template<typename Move_Type, typename ...Move_Args>
        void add_standard_legal_move(Move_Args... args)
        {
            auto move = std::make_unique<Move_Type>(args...);
            for(char file = 'a'; file <= 'h'; ++file)
            {
                for(int rank = 1; rank <= 8; ++rank)
                {
                    add_legal_move(move.get(), file, rank);
                }
            }

            possible_moves.push_back(std::move(move));
        }

    private:
        Color my_color;
        std::string symbol;

        // Contains pointers to move data
        static std::vector<std::unique_ptr<const Move>> possible_moves;

        // Holds lists of possible legal moves indexed by starting square (using Board::board_index())
        std::vector<std::vector<Complete_Move>> legal_moves;

        void add_legal_move(const Move* move, char file, int rank);
};

#endif // PIECE_H
