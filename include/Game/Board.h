#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <array>
#include <bitset>
#include <atomic>

#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"
#include "Players/Thinking.h"

class Clock;
class Game_Result;
class Player;

class Move;
class Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;
class Pawn_Move;

//! This class represents the physical chess board.
//
//! Each instance contains data to track the position of pieces, move history,
//! 3-fold repetition, 50-move draw status, and legal moves.
//!
//! Some abbreviation definitions:
//! - FEN: Forsyth-Edwards Notation: a succinct format for encoding the state of a board in text. (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
//! - PGN: Portable Game Notation: the standard way of recording moves in a game. (https://en.wikipedia.org/wiki/Portable_Game_Notation)
class Board
{
    public:
        explicit Board(const std::string& fen = standard_starting_fen); // reproduce board from Forsythe-Edwards Notation string

        Game_Result submit_move(const Move& move);

        const Move& create_move(const std::string& move) const;

        bool is_legal(Square start, Square end) const;

        Color whose_turn() const;

        void ascii_draw(Color perspective) const;
        std::string fen_status() const; // current state of board in FEN

        const std::vector<const Move*>& game_record() const;
        std::string last_move_record() const;

        static void set_thinking_mode(Thinking_Output_Type);
        static Thinking_Output_Type thinking_mode();
        static void pick_move_now();
        static bool must_pick_move_now();
        static void choose_move_at_leisure();

        void print_game_record(const Player* white,
                               const Player* black,
                               const std::string& file_name,
                               const Game_Result& result,
                               const Clock& game_clock,
                               const std::string& unusual_ending_reason = "") const;

        Piece piece_on_square(Square square) const;

        const std::vector<const Move*>& legal_moves() const;

        bool safe_for_king(Square square, Color king_color) const;
        bool blocked_attack(Square square, Color attacking_color) const;
        bool is_en_passant_targetable(Square square) const;
        bool piece_has_moved(Square square) const;
        Square find_king(Color color) const;
        bool king_is_in_check() const;
        bool king_is_in_check_after_move(const Move& move) const;
        bool piece_is_pinned(Square square) const;
        bool all_empty_between(Square start, Square end) const;
        bool enough_material_to_checkmate(Color color = NONE) const;
        bool move_captures(const Move& move) const;
        size_t moves_since_pawn_or_capture() const;
        const std::bitset<16>& moves_attacking_square(Square square, Color attacking_color) const;
        uint64_t board_hash() const;

        // Methods for gene reference
        bool last_move_captured() const;
        bool last_move_changed_material() const;
        bool material_change_possible() const;
        size_t castling_move_index(Color player) const;
        size_t attack_count(Color attacker) const;

        Board without_random_pawn() const;

    private:
        std::array<Piece, 64> board;
        std::array<uint64_t, 101> repeat_count;
        size_t repeat_count_insertion_point;
        Color turn_color;
        std::vector<const Move*> game_record_listing;
        std::array<bool, 64> unmoved_positions;
        Square en_passant_target;
        std::string starting_fen;
        static const std::string standard_starting_fen;
        std::array<Square, 2> king_location;
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

        void add_attacks_from(Square square, Piece piece);
        void remove_attacks_from(Square square, Piece old_piece);
        void modify_attacks(Square square, Piece piece, bool adding_attacks);
        void update_blocks(Square square, Piece old_piece, Piece new_piece);
        void adjust_attack_counts(Piece piece1, Piece piece2, Square square);
        void adjust_attack_counts_common(Piece piece1, Piece piece2, Square square, int adding_or_subtracting);
        const std::bitset<16>& checking_moves() const;

        // Information cache for gene reference
        bool material_change_move_available;
        std::array<size_t, 2> castling_index;
        std::array<size_t, 2> attack_counts;

        // Caches
        std::vector<const Move*> legal_moves_cache;

        void recreate_move_caches();

        // Communication channels
        static std::atomic<Thinking_Output_Type> thinking_indicator;
        static std::atomic_bool move_immediately;

        Piece& piece_on_square(Square square);
        void remove_piece(Square square);
        void move_piece(const Move& move);
        const Move& create_move(Square start, Square rank, char promote = 0) const;
        bool no_legal_moves() const;
        void make_en_passant_targetable(Square square);
        void clear_en_passant_target();
        void clear_checking_square();
        bool is_in_legal_moves_list(const Move& move) const;
        void place_piece(Piece piece, Square square);
        void switch_turn();
        void set_unmoved(Square square);
        void update_board(const Move& move);

        // Track threefold repetition and fifty-move rule
        void add_board_position_to_repeat_record();
        void add_to_repeat_count(uint64_t new_hash);
        ptrdiff_t current_board_position_repeat_count() const;
        void clear_repeat_count();

        // Zobrist hashing (implementation of threefold/fifty-move tracking)
        uint64_t current_board_hash;

        void update_board_hash(Square square);
        uint64_t square_hash(Square square) const;
        void update_whose_turn_hash();

        bool king_multiply_checked() const;

        [[noreturn]] void fen_error(const std::string& reason) const;

        // Moves with side effects are friends of Board
        friend class Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
