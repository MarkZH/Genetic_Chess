#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <utility>

class Kingside_Castle;
class Queenside_Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;
class Piece;

#include "Moves/Move.h"
#include "Color.h"

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

        void ascii_draw(Color perspective = WHITE) const;

        std::string fen_status() const; // current state of board in FEN
        const std::vector<std::string>& get_game_record() const;
        void print_game_record(const std::string& white_name,
                               const std::string& black_name,
                               const std::string& file_name = "",
                               const std::string& outside_result = "",
                               unsigned int game_number = 0) const;
        std::string board_status() const; // for detecting threefold repetition
        Color get_winner() const;
        std::string last_move() const;

        Board make_hypothetical() const;
        void set_turn(Color color);

        static bool inside_board(char file);
        static bool inside_board(int rank);

        std::shared_ptr<const Piece>& piece_on_square(char file, int rank);
        const std::shared_ptr<const Piece>& piece_on_square(char file, int rank) const;

        std::vector<Complete_Move> all_legal_moves() const;
        std::vector<Complete_Move> all_moves() const;
        bool square_attacked_by(char file, int rank, Color color) const;
        bool is_en_passant_targetable(char file, int rank) const;
        bool piece_has_moved(char file, int rank) const;
        bool piece_has_moved(const std::shared_ptr<const Piece>& piece) const;
        std::pair<char, int> find_king(Color color) const;
        bool king_is_in_check(Color color) const;

        bool game_has_ended() const;

    private:
        std::vector<std::shared_ptr<const Piece>> board;
        std::map<std::string, int> repeat_count;
        Color turn_color;
        std::vector<std::string> game_record;
        Color winner;
        bool is_original;
        std::map<std::shared_ptr<const Piece>, bool> piece_moved;
        char en_passant_target_file;
        int en_passant_target_rank;
        bool game_ended;

        // Caches
        mutable std::vector<Complete_Move> all_moves_cache;
        mutable std::vector<Complete_Move> all_legal_moves_cache;

        void place_piece(const std::shared_ptr<const Piece>& p, char file, int rank);
        void make_move(char file_start, int rank_start, char file_end, int rank_end);
        bool no_legal_moves() const;
        void set_winner(Color color);
        void reset_fifty_move_count();
        void make_en_passant_targetable(char file, int rank);
        void clear_en_passant_target();
        void all_pieces_unmoved();

        // Moves with side effects
        friend class Kingside_Castle; // moves second piece
        friend class Queenside_Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
