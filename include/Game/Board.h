#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>
#include <string>
#include <memory>

class Move;
class Complete_Move;
class Kingside_Castle;
class Queenside_Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;

#include "Square.h"
#include "Color.h"

enum algebraic_format {SHORT, LONG};

class Board
{
    public:
        Board();
        explicit Board(const std::string& fen); // reproduce board from FEN
                                                // if KQkq are missing, Rook relevant rook->player_moved()
                                                // make_en_passant_targetable()

        bool is_legal(char file_start, int rank_start, const std::shared_ptr<const Move>& move, bool king_check = true) const;
        bool is_legal(const Complete_Move& move, bool king_check = true) const;
        bool is_legal(char file_start, int rank_start, char file_end, int rank_end, bool king_check = true) const;

        void submit_move(char file_start, int rank_start, const std::shared_ptr<const Move>& move);
        void submit_move(const Complete_Move& cm);

        Complete_Move get_complete_move(const std::string& move, char promote = 0) const;
        Complete_Move get_complete_move(char file_start, int rank_start, char file_end, int rank_end, char promote = 0) const;

        Color whose_turn() const;

        void ascii_draw(Color color = WHITE) const;

        const Square& view_square(char file, int rank) const;

        std::string fen_status() const; // current state of board in FEN
        void print_game_record(const std::string& white_name,
                               const std::string& black_name,
                               const std::string& file_name = "",
                               const std::string& outside_result = "") const;
        std::string board_status() const; // for detecting threefold repetition
        Color get_winner() const;
        unsigned int number_of_moves() const;
        std::string last_move() const;

        Board make_hypothetical() const;
        void set_turn(Color color);

        static bool inside_board(char file);
        static bool inside_board(int rank);

        std::vector<Complete_Move> all_legal_moves() const;
        std::vector<Complete_Move> all_moves() const;
        int number_all_moves() const;
        bool square_attacked_by(char file, int rank, Color color) const;

    private:
        std::vector<Square> board;
        std::map<std::string, int> repeat_count;
        Color turn_color;
        std::vector<std::string> game_record;
        Color winner;
        bool is_original;

        // Caches
        mutable std::vector<Complete_Move> all_moves_cache;
        mutable std::vector<Complete_Move> all_legal_moves_cache;

        void place_piece(const std::shared_ptr<const Piece>& p, char file, int rank);
        bool king_is_in_check(Color color) const;
        Square& get_square(char file, int rank);
        void make_move(char file_start, int rank_start, char file_end, int rank_end);
        bool no_legal_moves() const;
        void set_winner(Color color);
        void reset_fifty_move_count();

        friend class Kingside_Castle;
        friend class Queenside_Castle;
        friend class En_Passant;
        friend class Pawn_Promotion;
        friend class Pawn_Double_Move;
        friend class Pawn_Move;
};

#endif // BOARD_H
