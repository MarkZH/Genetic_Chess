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
        //! Constructs a board according to an FEN string.
        //
        //! \param fen An optional text string given in FEN. If no argument is given,
        //!        then the FEN string for the start of a standard chess game is given.
        //! \throws std::invalid_argument Thrown if the FEN string does not represent a valid board state.
        explicit Board(const std::string& fen = standard_starting_fen); // reproduce board from Forsythe-Edwards Notation string

        //! Updates the state of the board according to a Player-selected Move.
        //
        //! \param move A Move-class instance. This must be an item taken from Board::legal_moves().
        //!        The input move is only checked for legality in DEBUG builds, where illegal moves
        //!        will trip an assert. In RELEASE builds, submitting an illegal move is undefined,
        //!        resulting in anything from an invalid board state to a crash due to segfault.
        //! \returns Returns a Game_Result indicating the result of the move and whether the game has ended.
        Game_Result submit_move(const Move& move);

        //! Creates a Move instance given a text string representation.
        //
        //! \param move A string using coordinate notation ("a2b3"), PGN ("Bb3"), or FEN. Note: If PGN is used and
        //!        a bishop is to be moved, then the piece symbol 'B' must be capitalized to avoid ambiguity
        //!        with pawn captures. For example, Bxc5 and bxc5. If the string is an FEN string corrsponding
        //!        to the position after the move, the method will search for a legal move that brings the board
        //!        into a state that matches the FEN.
        //! \returns A Move instance corresponding to the input string.
        //! \throws Illegal_Move if the text does not represent a legal move or if the wanted move is ambiguous.
        const Move& create_move(const std::string& move) const;

        //! Checks if there are any legal moves from the start sqaure to the end square.
        //
        //! \param start The square where the move should start.
        //! \param end   The square where the move should end.
        //! \returns True if there is a legal move between the given squares.
        bool is_legal(Square start, Square end) const;

        //! Tells which player is due to move.
        //
        //! \returns Color of player who is next to move.
        Color whose_turn() const;

        //! Prints an ASCII version of the board to a terminal.
        //
        //! This is useful for Human_Player on the terminal and debugging.
        //! \param perspective Specifies which side of the board is at the bottom of the screen.
        void ascii_draw(Color perspective) const;

        //! Returns the status of the game in FEN.
        //
        //! \returns A textual representation of the game state.
        //!
        //! This may slightly differ from the output of other programs
        //! in that the en passant target is only listed if there is a
        //! legal en passant move to be made.
        std::string fen_status() const; // current state of board in FEN

        //! Get the history of moves.
        //
        //! \returns The list of moves made on this board.
        const std::vector<const Move*>& game_record() const;

        //! Get last move for dispaly in text-based UIs.
        //
        //! \returns Last move on this board in PGN notation
        std::string last_move_record() const;

        //! Set the format an engine should output while picking a move.
        //
        //! \param mode Which chess engine protocol is being used: CECP, UCI, or NO_THINKING.
        static void set_thinking_mode(Thinking_Output_Type);

        //! Find out what kind of format an engine should output while picking a move.
        //
        //! \returns Format of thinking output: CECP, UCI, or NO_THINKING.
        static Thinking_Output_Type thinking_mode();

        //! Force the Player that is currently choosing a move to stop thinking and immediately make a move.
        static void pick_move_now();

        //! Check whether a Player should stop thinking and immediately move.
        static bool must_pick_move_now();

        //! Allow the Player to take any amount of time to choose a move.
        static void choose_move_at_leisure();

        //! Prints the PGN game record with commentary from Players.
        //
        //! \param white Pointer to Player playing white to provide commentary for moves. Can be nullptr.
        //! \param black Pointer to Player playing black to provide commentary for moves. Can be nullptr.
        //! \param file_name Name of the text file where the game will be printed. If empty, print to stdout.
        //! \param result The result of the last action (move, clock punch, or outside intervention) in the game.
        //! \param game_clock The game clock used during the game.
        //! \param unusual_ending_reason A reason for the game ending not covered by a chess rule or the game clock.
        //!        Usually comes from an exception what() message.
        void print_game_record(const Player* white,
                               const Player* black,
                               const std::string& file_name,
                               const Game_Result& result,
                               const Clock& game_clock,
                               const std::string& unusual_ending_reason = "") const;

        //! Get the piece on the square indicated by coordinates.
        //
        //! \param square The queried square.
        //! \returns Pointer to piece on square. May be nullptr if square is emtpy.
        Piece piece_on_square(Square square) const;

        //! Get a list of all legal moves for the current player.
        //
        //! \returns A list of pointers to legal moves. Any call to Board::submit_move() must take
        //!          its argument from this list.
        const std::vector<const Move*>& legal_moves() const;

        //! Find out whether the input square is safe for the given king to occupy.
        //
        //! A square is not safe for the king if the opposing pieces can attack the square.
        //! \param square The queried square.
        //! \param king_color The color of the king piece that is under potential attack.
        //! \returns Whether the king would be in check if it was placed on the square in question.
        bool safe_for_king(Square square, Color king_color) const;

        //! Determine if there are any attacks on a square that are blocked by other pieces.
        //
        //! \param square The queried square.
        //! \param attacking_color The color of the attacking pieces.
        //! \returns Whether there is an attack on the square that is blocked by another piece.
        bool blocked_attack(Square square, Color attacking_color) const;

        //! Determine whether the indicated square can be a target of an en passant move.
        //
        //! The is called by En_Passant::move_specific_legal().
        //! \param square The queried square.
        //! \returns Whether the square was passed by a pawn double move.
        bool is_en_passant_targetable(Square square) const;

        //! Indicates whether the queried square has a piece on it that never moved.
        //
        //! \param square The queried squaer.
        //! \returns If the piece on the square has never moved during the game.
        bool piece_has_moved(Square square) const;

        //! Finds the square on which a king resides.
        //
        //! \param color Which king to find.
        //! \returns Square which contains the sought after king.
        Square find_king(Color color) const;

        //! Find out if the king of the player to move is currently in check.
        //
        //! \returns If the current player is in check.
        bool king_is_in_check() const;

        //! Check if a move will leave the player making the move in check.
        //
        //! \param move A possibly legal move to check.
        //! \returns Whether the move under consideration will leave the friendly king in check.
        bool king_is_in_check_after_move(const Move& move) const;

        //! Determine whether a piece would be pinned to the king by an opposing piece if it was on the given square.
        //
        //! \param square The queried square.
        //! \returns Whether there is an opposing piece (of color opposite(Board::whose_turn()))
        //!          that would pin a piece occupying the queried square to its king. For example,
        //!          if it is white's turn, is there is a black piece that would pin a (possibly non-
        //!          existant) white piece on the given square to the white king?
        bool piece_is_pinned(Square square) const;

        //! Check whether all of the squares between two squares are empty.
        //
        //! This method assumes the squares are along the same rank, file, or diagonal.
        //! \param start The first square.
        //! \param end   The second square.
        //! \returns Whether all of the intervening squares between the two input squares are unoccupied by pieces.
        //! \throws assertion_failure In DEBUG builds, squares not along the same rank, file, or diagonal
        //!         will trigger an assertion failure. In RELEASE builds, the result is undefined.
        bool all_empty_between(Square start, Square end) const;

        //! Checks whether there are enough pieces on the board for any possible checkmate.
        //
        //! The following piece sets on the board make checkmate possible:
        //! - Any single pawn, rook, or queen,
        //! - At least two bishops (of any color) on oppositely colored squares,
        //! - A bishop and knight (of an piece color or square color combination),
        //! - Two knights (of any piece color or square color combination).
        //! \param color If NONE, check both sides for enough material. Otherwise, only check the pieces of one side.
        //! \returns If there are enough pieces on the board to make a checkmate arrangement.
        //!          If the method returns false when called with NONE, this will usually lead to a drawn game.
        bool enough_material_to_checkmate(Color color = NONE) const;

        //! Determines whether a move captures on the current board.
        //
        //! \param move Move to check.
        //! \returns If the move captures an opponent's piece.
        //! \throws assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
        bool move_captures(const Move& move) const;

        //! The number of moves since the last capture or pawn move.
        //
        //! \returns How many moves have been made since the last capturing or pawn move.
        size_t moves_since_pawn_or_capture() const;

        //! Get a list of all moves that attack a given square.
        //
        //! \param square The square being attacked.
        //! \param attacking_color The color of pieces doing the attacking.
        const std::bitset<16>& moves_attacking_square(Square square, Color attacking_color) const;

        //! Returns the Zobrist hash of the current state of the board.
        //
        //! See https://en.wikipedia.org/wiki/Zobrist_hashing for details.
        uint64_t board_hash() const;


        // Methods for gene reference

        //! Report if the most recent move in the game captured a piece.
        //
        //! This method is used by chess engines to decide how interesting a board position is
        //! and how much time to devote to future moves.
        //! \returns Whether the last move captured an opponent's piece.
        bool last_move_captured() const;

        //! Report if the last move changed the presence of pieces on the board.
        //
        //! This is more general than Board::last_move_captured() since it also checks
        //! for pawn promotions.
        //! \returns Whether the last move captured or resulted in a promoted pawn.
        bool last_move_changed_material() const;

        //! Check if any currently legal move changes material by capturing or promoting a pawn.
        //
        //! This method is used by chess engines to determine how interesting a board position
        //! is and thus how much time to devote to studying future moves.
        //! \returns If any legal move captures or promotes a pawn.
        bool material_change_possible() const;

        //! Gets the ply move during which a player castled.
        //
        //! \param player The color of the player being queried.
        //! \returns The ply count when the player castled, or MAX(size_t) if the player has not castled.
        size_t castling_move_index(Color player) const;

        //! Returns the number of attacking moves available.
        //
        //! The method is used in the Freedom_To_Move_Gene::score_board() method.
        //! \param attacking_color The color of pieces doing the attacking.
        //! \returns The number of attacking moves excepting those that attack pieces
        //!          of the same color.
        size_t attack_count(Color attacking_color) const;

        //! Create a copy of the board with a random pawn removed.
        //
        //! This can be used by a Player to calibrate the value of a centipawn.
        //! \throws Debug assertion failure if there are no pawns on the board.
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
        mutable Square last_pin_check_square;
        mutable bool last_pin_result;
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
