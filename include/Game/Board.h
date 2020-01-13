#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <array>
#include <bitset>

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
        //! Constructs a board in the standard starting position.
        Board() noexcept;

        //! Constructs a board according to an FEN string.
        //
        //! \param fen An text string given in FEN.
        //! \throws std::invalid_argument Thrown if the FEN string does not represent a valid board state.
        explicit Board(const std::string& fen);

        //! Updates the state of the board according to a Player-selected Move.
        //
        //! \param move A Move-class instance. This must be an item taken from Board::legal_moves().
        //!        The input move is only checked for legality in DEBUG builds, where illegal moves
        //!        will trip an assert. In RELEASE builds, submitting an illegal move is undefined,
        //!        resulting in anything from an invalid board state to a crash due to segfault.
        //! \returns Returns a Game_Result indicating the result of the move and whether the game has ended.
        Game_Result submit_move(const Move& move) noexcept;

        //! Updates the state of the board according to the text-based move.
        //
        //! This is equivalent to calling board.submit_move(board.create_move(move)).
        //! \param move A text string specifying a move in any notation that uniquely identifies a
        //!        legal move (PGN, coordinate, etc.).
        //! \returns A Game_Result indicating the result of the move and whether the game has ended.
        //! \throws Illegal_Move if the text represents an illegal or ambiguous move.
        Game_Result submit_move(const std::string& move);

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
        bool is_legal(Square start, Square end) const noexcept;

        //! Tells which player is due to move.
        //
        //! \returns Color of player who is next to move.
        Color whose_turn() const noexcept;

        //! Prints an ASCII version of the board to a terminal.
        //
        //! This is useful for Human_Player on the terminal and debugging.
        //! \param perspective Specifies which side of the board is at the bottom of the screen.
        void ascii_draw(Color perspective) const noexcept;

        //! Returns the status of the game in FEN.
        //
        //! \returns A textual representation of the game state.
        //!
        //! This may slightly differ from the output of other programs
        //! in that the en passant target is only listed if there is a
        //! legal en passant move to be made.
        std::string fen() const noexcept;

        //! Get the history of moves.
        //
        //! \returns The list of moves made on this board.
        const std::vector<const Move*>& game_record() const noexcept;

        //! Set the format an engine should output while picking a move.
        //
        //! \param mode Which chess engine protocol is being used: CECP, UCI, or NO_THINKING.
        static void set_thinking_mode(Thinking_Output_Type) noexcept;

        //! Find out what kind of format an engine should output while picking a move.
        //
        //! \returns Format of thinking output: CECP, UCI, or NO_THINKING.
        static Thinking_Output_Type thinking_mode() noexcept;

        //! Force the Player that is currently choosing a move to stop thinking and immediately make a move.
        static void pick_move_now() noexcept;

        //! Check whether a Player should stop thinking and immediately move.
        static bool must_pick_move_now() noexcept;

        //! Allow the Player to take any amount of time to choose a move.
        static void choose_move_at_leisure() noexcept;

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
                               const std::string& unusual_ending_reason = "") const noexcept;

        //! Get the piece on the square indicated by coordinates.
        //
        //! \param square The queried square.
        //! \returns Piece instance on square. May be an invalid Piece if square is emtpy.
        Piece piece_on_square(Square square) const noexcept;

        //! Get a list of all legal moves for the current player.
        //
        //! \returns A list of pointers to legal moves. Any call to Board::submit_move() must take
        //!          its argument from this list.
        const std::vector<const Move*>& legal_moves() const noexcept;

        //! Find out whether the input square is safe for the given king to occupy.
        //
        //! A square is not safe for the king if the opposing pieces can attack the square.
        //! \param square The queried square.
        //! \param king_color The color of the king piece that is under potential attack.
        //! \returns Whether the king would be in check if it was placed on the square in question.
        bool safe_for_king(Square square, Color king_color) const noexcept;

        //! Determine if there are any attacks on a square that are blocked by other pieces.
        //
        //! \param square The queried square.
        //! \param attacking_color The color of the attacking pieces.
        //! \returns Whether there is an attack on the square that is blocked by another piece.
        bool blocked_attack(Square square, Color attacking_color) const noexcept;

        //! Indicates whether the queried square has a piece on it that never moved.
        //
        //! \param square The queried squaer.
        //! \returns If the piece on the square has never moved during the game.
        bool piece_has_moved(Square square) const noexcept;

        //! Finds the square on which a king resides.
        //
        //! \param color Which king to find.
        //! \returns Square which contains the sought after king.
        Square find_king(Color color) const noexcept;

        //! Find out if the king of the player to move is currently in check.
        //
        //! \returns If the current player is in check.
        bool king_is_in_check() const noexcept;

        //! Check if a move will leave the player making the move in check.
        //
        //! \param move A possibly legal move to check.
        //! \returns Whether the move under consideration will leave the friendly king in check.
        bool king_is_in_check_after_move(const Move& move) const noexcept;

        //! Determine whether a piece would be pinned to the king by an opposing piece if it was on the given square.
        //
        //! \param square The queried square.
        //! \returns Whether there is an opposing piece (of color opposite(Board::whose_turn()))
        //!          that would pin a piece occupying the queried square to its king. For example,
        //!          if it is white's turn, is there is a black piece that would pin a (possibly non-
        //!          existant) white piece on the given square to the white king?
        bool piece_is_pinned(Square square) const noexcept;

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
        bool enough_material_to_checkmate(Color color = NONE) const noexcept;

        //! Determines whether a move will capture on the current board.
        //
        //! \param move Move to check.
        //! \returns If the move captures an opponent's piece.
        //! \throws assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
        bool move_captures(const Move& move) const noexcept;

        //! Determines whether a move will change the material on the current board.
        //
        //! \param move Move to check.
        //! \returns If the move captures an opponent's piece or promotes a pawn.
        //! \throws assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
        bool move_changes_material(const Move& move) const noexcept;

        //! Returns the Zobrist hash of the current state of the board.
        //
        //! See https://en.wikipedia.org/wiki/Zobrist_hashing for details.
        uint64_t board_hash() const noexcept;

        //! Gets the ply move during which a player castled.
        //
        //! \param player The color of the player being queried.
        //! \returns The ply count when the player castled, or MAX(size_t) if the player has not castled.
        size_t castling_move_index(Color player) const noexcept;

        //! Create a copy of the board with a random pawn removed.
        //
        //! This can be used by a Player to calibrate the value of a centipawn.
        //! \throws Debug assertion failure if there are no pawns on the board.
        Board without_random_pawn() const noexcept;

        //! Returns a list of moves that results in a quiescent version of the board.
        //
        //! This list is created by performing all possible captures on
        //! the ending square of the last move. If multiple captures are
        //! possible, the weakest pieces goes first according to the piece
        //! values array.
        //! \param piece_values An array indexed by Piece::type() that gives
        //!        the value of the piece.
        std::vector<const Move*> quiescent(const std::array<double, 6>& piece_values) const noexcept;

        //! Returns the number of moves available to the opponent prior to the opponent's last move.
        //
        //! Returns 0 if no moves have been made on the board.
        size_t previous_moves_count() const noexcept;

    private:
        std::array<Piece, 64> board;
        std::array<uint64_t, 101> repeat_count;
        size_t repeat_count_insertion_point = 0;
        Color turn_color;
        std::vector<const Move*> game_record_listing;
        std::array<bool, 64> unmoved_positions{};
        Square en_passant_target;
        std::string starting_fen;
        std::array<Square, 2> king_location;
        Square checking_square;
        mutable Square last_pin_check_square;
        mutable bool last_pin_result;
        size_t move_count_start_offset;

        // Stores the moves that attack a square. The innermost array
        // is filled with bools indicating the direction the piece attacking
        // the square moves to reach that square: (from white's perspective)
        // up, down, left, right, up-left, up-right, down-left, down-right,
        // 2x1 up-left, 1x2 up-left, 2x1 up-right, 1x2 up-right,
        // 2x1 down-left, 1x2 down-left, 2x1 down-right, 1x2 down-right
        std::array<std::array<std::bitset<16>, 64>, 2> potential_attacks{}; // indexed by [attacker color][square index];
        std::array<std::array<std::bitset<16>, 64>, 2> blocked_attacks{};

        void add_attacks_from(Square square, Piece piece) noexcept;
        void remove_attacks_from(Square square, Piece old_piece) noexcept;
        void modify_attacks(Square square, Piece piece, bool adding_attacks) noexcept;
        void update_blocks(Square square, Piece old_piece, Piece new_piece) noexcept;
        const std::bitset<16>& moves_attacking_square(Square square, Color attacking_color) const noexcept;
        const std::bitset<16>& checking_moves() const noexcept;
        Square find_checking_square() const noexcept;

        // Information cache for gene reference
        std::array<size_t, 2> castling_index{size_t(-1), size_t(-1)};

        // Caches
        std::vector<const Move*> legal_moves_cache;
        size_t prior_moves_count = 0;

        void recreate_move_caches() noexcept;

        Piece& piece_on_square(Square square) noexcept;
        void remove_piece(Square square) noexcept;
        void move_piece(const Move& move) noexcept;
        const Move& create_move(Square start, Square rank, char promote = 0) const;
        bool no_legal_moves() const noexcept;
        void make_en_passant_targetable(Square square) noexcept;
        void clear_en_passant_target() noexcept;
        bool is_en_passant_targetable(Square square) const noexcept;
        bool is_in_legal_moves_list(const Move& move) const noexcept;
        void place_piece(Piece piece, Square square) noexcept;
        bool all_empty_between(Square start, Square end) const noexcept;
        void set_unmoved(Square square) noexcept;
        void update_board(const Move& move) noexcept;
        Game_Result move_result() const noexcept;

        // Track threefold repetition and fifty-move rule
        void add_board_position_to_repeat_record() noexcept;
        void add_to_repeat_count(uint64_t new_hash) noexcept;
        ptrdiff_t current_board_position_repeat_count() const noexcept;
        void clear_repeat_count() noexcept;
        size_t moves_since_pawn_or_capture() const noexcept;

        // Zobrist hashing (implementation of threefold/fifty-move tracking)
        uint64_t current_board_hash = 0;

        void update_board_hash(Square square) noexcept;
        uint64_t square_hash(Square square) const noexcept;
        void update_whose_turn_hash() noexcept;

        bool king_multiply_checked() const noexcept;

        [[noreturn]] void fen_error(const std::string& reason) const;

        // Moves with side effects are friends of Board
        friend class Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
