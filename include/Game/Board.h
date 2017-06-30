#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <set>

#include "Moves/Complete_Move.h"
#include "Color.h"
#include "Game_Result.h"
#include "Players/Player.h"
#include "Players/Thinking.h"

class Piece;
class Clock;

struct Square
{
    char file;
    int rank;

    bool operator<(const Square& other) const;
    operator bool() const;
};

// The distance between two squares in king moves
int king_distance(const Square& a, const Square& b);


class Board
{
    public:
        Board();
        explicit Board(const std::string& fen); // reproduce board from Forsythe-Edwards Notation string

        Game_Result submit_move(const Complete_Move& cm);

        Complete_Move get_complete_move(const std::string& move, char promote = 0) const;
        Complete_Move get_complete_move(char file_start, int rank_start, char file_end, int rank_end, char promote = 0) const;

        Color whose_turn() const;

        void ascii_draw(Color perspective = WHITE) const;

        std::string fen_status() const; // current state of board in FEN
        const std::vector<std::string>& get_game_record() const;
        std::string get_last_move_record() const;

        // Communication between players
        void set_thinking_mode(Thinking_Output_Type) const;
        Thinking_Output_Type get_thinking_mode() const;

        // With commentary
        void print_game_record(const Player* white,
                               const Player* black,
                               const std::string& file_name,
                               Game_Result result,
                               double initial_time,
                               size_t moves_to_reset,
                               double increment,
                               const Clock& game_clock) const;

        std::string last_move_coordinates() const;

        void set_turn(Color color);

        static bool inside_board(char file, int rank);
        static bool inside_board(char file);
        static bool inside_board(int rank);

        const Piece* view_piece_on_square(char file, int rank) const;

        const std::vector<Complete_Move>& legal_moves() const;
        const std::vector<Complete_Move>& other_moves() const;

        bool safe_for_king(char file, int rank, Color king_color) const;
        bool is_en_passant_targetable(char file, int rank) const;
        bool piece_has_moved(char file, int rank) const;
        Square find_king(Color color) const;
        bool king_is_in_check(Color color) const;

    private:
        std::vector<std::shared_ptr<const Piece>> board;
        std::map<std::string, int> repeat_count;
        Color turn_color;
        std::vector<std::string> game_record;
        std::set<Square> unmoved_positions;
        Square en_passant_target;
        std::string starting_fen;

        // Caches
        mutable std::vector<Complete_Move> other_moves_cache;
        mutable std::vector<Complete_Move> legal_moves_cache;
        void clear_caches();

        // Communication channels
        mutable Thinking_Output_Type thinking_indicator;

        std::shared_ptr<const Piece>& piece_on_square(char file, int rank);
        void remove_piece(char file, int rank);
        void make_move(char file_start, int rank_start, char file_end, int rank_end);
        bool no_legal_moves() const;
        void reset_fifty_move_count();
        std::string board_status() const; // for detecting threefold repetition
        void make_en_passant_targetable(char file, int rank);
        void clear_en_passant_target();
        bool enough_material_to_checkmate() const;
        static Color square_color(char file, int rank);
        static size_t board_index(char file, int rank);
        bool is_legal(char file_start, int rank_start,
                      char file_end,   int rank_end) const;

        // Moves with side effects
        friend class Kingside_Castle; // moves second piece
        friend class Queenside_Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts

        friend class Move; // needs access to check if King is in check after a move
};

#endif // BOARD_H
