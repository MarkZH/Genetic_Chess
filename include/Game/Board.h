#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <array>
#include <mutex>

#include "Color.h"
#include "Square.h"
#include "Pieces/Piece_Types.h"
#include "Players/Thinking.h"

class Clock;
class Game_Result;
class Player;

class Piece;
class Pawn;
class Rook;
class Knight;
class Bishop;
class Queen;
class King;

class Move;
class Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;
class Pawn_Move;


class Board
{
    public:
        Board();
        explicit Board(const std::string& fen); // reproduce board from Forsythe-Edwards Notation string

        Game_Result submit_move(const Move& move);

        const Move& create_move(const std::string& move) const;
        const Move& create_move(char file_start, int rank_start, char file_end, int rank_end, char promote = 0) const;

        bool is_legal(char file_start, int rank_start,
                      char file_end,   int rank_end) const;

        Color whose_turn() const;
        static Color square_color(char file, int rank);

        void ascii_draw(Color perspective = WHITE) const;

        std::string fen_status() const; // current state of board in FEN
        const std::vector<const Move*>& game_record() const;
        std::string last_move_record() const;

        // Communication between players
        void set_thinking_mode(Thinking_Output_Type) const;
        Thinking_Output_Type thinking_mode() const;

        // With commentary
        void print_game_record(const Player* white,
                               const Player* black,
                               const std::string& file_name,
                               const Game_Result& result,
                               double initial_time,
                               size_t moves_to_reset,
                               double increment,
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
        bool is_en_passant_targetable(char file, int rank) const;
        bool piece_has_moved(char file, int rank) const;
        Square find_king(Color color) const;
        bool king_is_in_check() const;
        bool king_is_in_check_after_move(const Move& move) const;
        Square piece_is_pinned(char file, int rank) const; // returns pinning square or {'\0', 0} if none
        bool all_empty_between(char file_start, int rank_start, char file_end, int rank_end) const;
        bool enough_material_to_checkmate() const;
        bool move_captures(const Move& move) const;

        // Methods for gene reference
        bool capture_possible() const;
        const std::array<bool, 64>& all_square_indices_attacked() const;
        const std::array<bool, 64>& other_square_indices_attacked() const;
        size_t castling_move_index(Color player) const;

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
        std::array<Square, 2> king_location;
        size_t move_count_start_offset;

        // Information cache for gene reference
        std::array<bool, 64> attacked_indices;
        std::array<bool, 64> other_attacked_indices;
        bool capturing_move_available;
        std::array<size_t, 2> castling_index;

        // Pieces
        static const Rook   white_rook;
        static const Knight white_knight;
        static const Bishop white_bishop;
        static const Queen  white_queen;
        static const King   white_king;
        static const Pawn   white_pawn;

        static const Rook   black_rook;
        static const Knight black_knight;
        static const Bishop black_bishop;
        static const Queen  black_queen;
        static const King   black_king;
        static const Pawn   black_pawn;

        // Caches
        std::vector<const Move*> legal_moves_cache;
        std::vector<Square> checking_squares;

        void recreate_move_caches();
        void refresh_checking_squares();

        // Communication channels
        mutable Thinking_Output_Type thinking_indicator;

        const Piece*& piece_on_square(char file, int rank);
        void remove_piece(char file, int rank);
        void make_move(char file_start, int rank_start, char file_end, int rank_end);
        bool no_legal_moves() const;
        std::string board_status() const;
        void make_en_passant_targetable(char file, int rank);
        void clear_en_passant_target();
        bool is_in_legal_moves_list(const Move& move) const;
        void place_piece(const Piece* piece, char file, int rank);
        void switch_turn();
        void set_unmoved(char file, int rank);
        void update_board(const Move& move);

        // Track threefold repetition and fifty-move rule
        void add_board_position_to_repeat_record();
        void add_to_repeat_count(uint64_t new_hash);
        int current_board_position_repeat_count() const;
        void clear_repeat_count();
        int moves_since_pawn_or_capture() const;

        // Zobrist hashing
        uint64_t current_board_hash;

        void initialize_board_hash();
        uint64_t board_hash() const;

        // Hash values for squares
        static std::mutex hash_lock;
        static bool hash_values_initialized;

        // [board_index][square_hash_index()] --> [piece_hash] (13 == number of piece types (black and white) + empty
        static std::array<std::array<uint64_t, 13>, 64> square_hash_values;

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
        bool attacks(char origin_file, int origin_rank, char target_file, int target_rank) const;

        // Moves with side effects are friends of Board
        friend class Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
