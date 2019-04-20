#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <array>
#include <bitset>

#include "Color.h"
#include "Square.h"
#include "Game/Piece_Types.h"
#include "Players/Thinking.h"

class Clock;
class Game_Result;
class Player;

class Piece;

class Move;
class Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;
class Pawn_Move;

//! This class represents the physical chess board.

//! Each instance contains data to track the position of pieces, move history,
//! 3-fold repetition, 50-move draw status, and legal moves.
//!
//! Some abbreviation definitions:
//! - FEN: Forsyth-Edwards notation: a succinct format for encoding the state of a board in text. (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
//! - PGN: Portable Game Notation: the standard way of recording moves in a game. (https://en.wikipedia.org/wiki/Portable_Game_Notation)
class Board
{
    public:
        explicit Board(const std::string& fen = standard_starting_fen); // reproduce board from Forsythe-Edwards Notation string

        Game_Result submit_move(const Move& move);

        const Move& create_move(const std::string& move) const;

        bool is_legal(char file_start, int rank_start,
                      char file_end,   int rank_end) const;

        Color whose_turn() const;
        static Color square_color(char file, int rank);

        void ascii_draw(Color perspective) const;
        std::string fen_status() const; // current state of board in FEN

        const std::vector<const Move*>& game_record() const;
        std::string last_move_record() const;

        void set_thinking_mode(Thinking_Output_Type) const;
        Thinking_Output_Type thinking_mode() const;

        void print_game_record(const Player* white,
                               const Player* black,
                               const std::string& file_name,
                               const Game_Result& result,
                               const Clock& game_clock) const;

        std::string last_move_coordinates() const;

        void set_turn(Color color);

        static bool inside_board(char file, int rank);
        static bool inside_board(char file);
        static bool inside_board(int rank);

        static size_t square_index(char file, int rank);

        const Piece* piece_on_square(char file, int rank) const;

        const std::vector<const Move*>& legal_moves() const;

        bool safe_for_king(char file, int rank, Color king_color) const;
        bool blocked_attack(char file, int rank, Color attacking_color) const;
        bool is_en_passant_targetable(char file, int rank) const;
        bool piece_has_moved(char file, int rank) const;
        const Square& find_king(Color color) const;
        bool king_is_in_check() const;
        bool king_is_in_check_after_move(const Move& move) const;
        bool piece_is_pinned(char file, int rank) const;
        bool all_empty_between(char file_start, int rank_start, char file_end, int rank_end) const;
        bool enough_material_to_checkmate(Color color = NONE) const;
        bool move_captures(const Move& move) const;
        int moves_since_pawn_or_capture() const;
        const std::bitset<16>& moves_attacking_square(char file, int rank, Color attacking_color) const;

        // Methods for gene reference
        bool last_move_captured() const;
        bool last_move_changed_material() const;
        bool material_change_possible() const;
        size_t castling_move_index(Color player) const;
        int attack_count(Color attacker) const;

        Board without_random_pawn() const;

        static const Piece* piece_instance(Piece_Type piece_type, Color color);

    private:
        std::array<const Piece*, 64> board;
        std::array<uint64_t, 101> repeat_count;
        int repeat_count_insertion_point;
        Color turn_color;
        std::vector<const Move*> game_record_listing;
        std::array<bool, 64> unmoved_positions;
        Square en_passant_target;
        std::string starting_fen;
        static const std::string standard_starting_fen;
        std::array<Square, 2> king_location;
        mutable std::array<bool, 64> pinned_squares;
        mutable std::array<bool, 64> square_searched_for_pin;
        mutable Square checking_square;
        size_t move_count_start_offset;

        // Stores the moves that attack a square. The innermost array
        // is filled with bools indicating the direction the piece attacking
        // the square moves to reach that square: (from white's perspective)
        // up, down, left, right, up-left, up-right, down-left, down-right,
        // 2x1 up-left, 1x2 up-left, 2x1 up-right, 1x2 up-right,
        // 2x1 down-left, 1x2 down-left, 2x1 down-right, 1x2 down-right
        std::array<std::array<std::bitset<16>, 64>, 2> potential_attacks; // indexed by [attacker color][square index];
        std::array<std::array<std::bitset<16>, 64>, 2> blocked_attacks;

        void add_attacks_from(char file, int rank, const Piece* piece);
        void remove_attacks_from(char file, int rank, const Piece* old_piece);
        void modify_attacks(char file, int rank, const Piece* piece, bool adding_attacks);
        void update_blocks(char file, int rank, const Piece* old_piece, const Piece* new_piece);
        const std::bitset<16>& checking_moves() const;

        // Information cache for gene reference
        bool material_change_move_available;
        std::array<size_t, 2> castling_index;
        std::array<int, 2> attack_counts;

        // Pieces
        static const Piece white_rook;
        static const Piece white_knight;
        static const Piece white_bishop;
        static const Piece white_queen;
        static const Piece white_king;
        static const Piece white_pawn;

        static const Piece black_rook;
        static const Piece black_knight;
        static const Piece black_bishop;
        static const Piece black_queen;
        static const Piece black_king;
        static const Piece black_pawn;

        // Caches
        std::vector<const Move*> legal_moves_cache;

        void recreate_move_caches();

        // Communication channels
        mutable Thinking_Output_Type thinking_indicator;

        const Piece*& piece_on_square(char file, int rank);
        void remove_piece(char file, int rank);
        void make_move(char file_start, int rank_start, char file_end, int rank_end);
        const Move& create_move(char file_start, int rank_start, char file_end, int rank_end, char promote = 0) const;
        bool no_legal_moves() const;
        void make_en_passant_targetable(char file, int rank);
        void clear_en_passant_target();
        void clear_pinned_squares();
        void clear_checking_square();
        bool is_in_legal_moves_list(const Move& move) const;
        void place_piece(const Piece* piece, char file, int rank);
        void switch_turn();
        void set_unmoved(char file, int rank);
        void update_board(const Move& move);

        // Track threefold repetition and fifty-move rule
        void add_board_position_to_repeat_record();
        void add_to_repeat_count(uint64_t new_hash);
        size_t current_board_position_repeat_count() const;
        void clear_repeat_count();

        // Zobrist hashing
        uint64_t current_board_hash;

        void initialize_board_hash();
        uint64_t board_hash() const;

        // Hash values for squares (static so copying isn't necessary)
            // [square_index()][square_hash_index()] --> [piece_hash] (13 == number of piece types (black and white) + empty)
        static std::array<std::array<uint64_t, 13>, 64> square_hash_values;
            // indexed by square_index()
        static std::array<uint64_t, 64> en_passant_hash_values;
        static std::array<uint64_t, 64> castling_hash_values;

        void update_board_hash(char file, int rank);
        uint64_t square_hash(char file, int rank) const;
        static size_t square_hash_index(const Piece* piece);

        // Whose turn?
        static uint64_t switch_turn_board_hash;
        void update_whose_turn_hash();

        bool king_multiply_checked() const;
        static bool straight_line_move(char file_start, int rank_start, char file_end, int rank_end);
        static bool moves_are_parallel(int file_change_1, int rank_change_1, int file_change_2, int rank_change_2);
        static bool same_direction(int file_change_1, int rank_change_1, int file_change_2, int rank_change_2);

        [[noreturn]] void fen_error(const std::string& reason) const;

        // Moves with side effects are friends of Board
        friend class Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
