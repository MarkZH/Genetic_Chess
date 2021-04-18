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

#include "Utility/Fixed_Capacity_Vector.h"

class Clock;
class Game_Result;
class Player;

class Move;
class Castle;
class En_Passant;
class Pawn_Promotion;
class Pawn_Double_Move;
class Pawn_Move;

//! \brief This class represents the physical chess board.
//!
//! Each instance contains data to track the position of pieces, move history,
//! 3-fold repetition, 50-move draw status, and legal moves.
//!
//! Some abbreviation definitions:
//! - FEN: Forsyth-Edwards Notation: a succinct format for encoding the state of a board in text. (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
//! - PGN: Portable Game Notation: the standard way of recording moves in a game. (https://en.wikipedia.org/wiki/Portable_Game_Notation)
class Board
{
    public:
        //! \brief Constructs a board in the standard starting position.
        Board() noexcept;

        //! \brief Constructs a board according to an FEN string.
        //!
        //! \param fen An text string given in FEN.
        //! \exception std::invalid_argument Thrown if the FEN string does not represent a valid board state.
        explicit Board(const std::string& fen);

        //! \brief Updates the state of the board according to a Player-selected Move.
        //!
        //! \param move A Move-class instance. This must be an item taken from Board::legal_moves().
        //!        The input move is only checked for legality in DEBUG builds, where illegal moves
        //!        will trip an assert. In RELEASE builds, playing an illegal move is undefined,
        //!        resulting in anything from an invalid board state to a crash due to segfault.
        //! \returns Returns a Game_Result indicating the result of the move and whether the game has ended.
        Game_Result play_move(const Move& move) noexcept;

        //! \brief Updates the state of the board according to the text-based move.
        //!
        //! This is equivalent to calling board.play_move(board.interpret_move(move)).
        //! \param move A text string specifying a move in any notation that uniquely identifies a
        //!        legal move (PGN, coordinate, etc.).
        //! \returns A Game_Result indicating the result of the move and whether the game has ended.
        //! \exception Illegal_Move if the text represents an illegal or ambiguous move. The Board is unchanged in this event.
        Game_Result play_move(const std::string& move);

        //! \brief Figure out a move sequence to get from the current board state to the new state given by the parameter.
        //!
        //! \param new_fen The new board state to be reached.
        //! \returns A list of moves that will result in the desired board state. An empty list will be returned
        //!          if no sequence of moves (maximum of 2) can be found.
        std::vector<const Move*> derive_moves(const std::string& new_fen) const noexcept;

        //! \brief Creates a Move instance given a text string representation.
        //!
        //! \param move A string using coordinate notation ("a2b3")
        //!        or [algebraic notation](https://en.wikipedia.org/wiki/Algebraic_notation_(chess)) ("Bb3").
        //! \returns A Move instance corresponding to the input string.
        //! \exception Illegal_Move if the text does not represent a legal move or if the wanted move is ambiguous.
        const Move& interpret_move(std::string move) const;

        //! \brief Tells which player is due to move.
        //!
        //! \returns Color of player who is next to move.
        Piece_Color whose_turn() const noexcept;

        //! \brief Returns the status of the game in FEN.
        //!
        //! \returns A textual representation of the game state.
        //!
        //! This may slightly differ from the output of other programs
        //! in that the en passant target is only listed if there is a
        //! legal en passant move to be made.
        std::string fen() const noexcept;

        //! \brief Returns the FEN string that was used to create the Board.
        std::string original_fen() const noexcept;

        //! \brief Returns the last move made on this Board
        //!
        //! \returns A pointer representing the last move made.
        const Move* last_move() const noexcept;

        //! \brief The length of the game on this Board.
        //!
        //! \returns The number of moves made on this board since its creation.
        size_t game_length() const noexcept;

        //! \brief The number of moves since this start of the game on this board.
        //!
        //! \returns The number of moves in this game including those before the creation of the board.
        //!
        //! The quantity can start as greater than zero if the FEN string used to create the board has
        //! a full-move clock value (the sixth field) of more than 1 or if black is the first to move.
        //! The calculation assumes that every game starts with white's move, even if the FEN string
        //! indicates that black is the first to move, since this would indicate a game state that is
        //! after the start of the game.
        size_t ply_count() const noexcept;

        //! \brief The direction (if any) of the castling move made by a player.
        //!
        //! \param player The color of the player being queried.
        //! \returns A number indicating the direction of the castling
        //!          (positive for kingside, negative for queenside, zero
        //!          for no castling yet).
        int castling_direction(Piece_Color player) const noexcept;

        //! \brief Set the format an engine should output while picking a move.
        //!
        //! \param mode Which chess engine protocol is being used: CECP, UCI, or NO_THINKING.
        static void set_thinking_mode(Thinking_Output_Type) noexcept;

        //! \brief Find out what kind of format an engine should output while picking a move.
        //!
        //! \returns Format of thinking output: CECP, UCI, or NO_THINKING.
        static Thinking_Output_Type thinking_mode() noexcept;

        //! \brief Force the Player that is currently choosing a move to stop thinking and immediately make a move.
        static void pick_move_now() noexcept;

        //! \brief Check whether a Player should stop thinking and immediately move.
        static bool must_pick_move_now() noexcept;

        //! \brief Allow the Player to take any amount of time to choose a move.
        static void choose_move_at_leisure() noexcept;

        //! \brief Prints the PGN game record with commentary from Players.
        //!
        //! \param game_record_listing A list of Moves. This must be a legal sequence of moves starting from
        //!        the state of the Board at its creation and resulting in the current state of the Board.
        //! \param white Pointer to Player playing white to provide commentary for moves. Can be nullptr.
        //! \param black Pointer to Player playing black to provide commentary for moves. Can be nullptr.
        //! \param file_name Name of the text file where the game will be printed. If empty, print to stdout.
        //! \param result The result of the last action (move, clock punch, or outside intervention) in the game.
        //! \param game_clock The game clock used during the game.
        //! \param event_name The name of the event where the game will take place. May be empty.
        //! \param location The name of the location of the game. May be empty.
        void print_game_record(const std::vector<const Move*>& game_record_listing,
                               const Player& white,
                               const Player& black,
                               const std::string& file_name,
                               const Game_Result& result,
                               const Clock& game_clock,
                               const std::string& event_name,
                               const std::string& location) const noexcept;

        //! \brief Get the piece on the square indicated by coordinates.
        //!
        //! \param square The queried square.
        //! \returns Piece instance on square. May be an invalid Piece if square is emtpy.
        Piece piece_on_square(Square square) const noexcept;

        //! \brief Get a list of all legal moves for the current player.
        //!
        //! \returns A list of pointers to legal moves. Any call to Board::play_move() must take
        //!          its argument from this list.
        const std::vector<const Move*>& legal_moves() const noexcept;

        //! \brief Find out whether the input square is safe for the given king to occupy.
        //!
        //! A square is not safe for the king if the opposing pieces can attack the square.
        //! \param square The queried square.
        //! \param king_color The color of the king piece that is under potential attack.
        //! \returns Whether the king would be in check if it was placed on the square in question.
        bool safe_for_king(Square square, Piece_Color king_color) const noexcept;

        //! \brief Determine if there are any attacks on a square that are blocked by other pieces.
        //!
        //! \param square The queried square.
        //! \param attacking_color The color of the attacking pieces.
        //! \returns Whether there is an attack on the square that is blocked by another piece.
        bool blocked_attack(Square square, Piece_Color attacking_color) const noexcept;

        //! \brief Indicates whether the queried square has a piece on it that never moved.
        //!
        //! \param square The queried squaer.
        //! \returns If the piece on the square has never moved during the game.
        bool piece_has_moved(Square square) const noexcept;

        //! \brief Finds the square on which a king resides.
        //!
        //! \param color Which king to find.
        //! \returns Square which contains the sought after king.
        Square find_king(Piece_Color color) const noexcept;

        //! \brief Find out if the king of the player to move is currently in check.
        //!
        //! \returns If the current player is in check.
        bool king_is_in_check() const noexcept;

        //! \brief Check if a move will leave the player making the move in check.
        //!
        //! \param move A possibly legal move to check.
        //! \returns Whether the move under consideration will leave the friendly king in check.
        bool king_is_in_check_after_move(const Move& move) const noexcept;

        //! \brief Determine whether a piece would be pinned to the king by an opposing piece if it was on the given square.
        //!
        //! \param square The queried square.
        //! \returns Whether there is an opposing piece (of color opposite(Board::whose_turn()))
        //!          that would pin a piece occupying the queried square to its king. For example,
        //!          if it is white's turn, is there is a black piece that would pin a (possibly non-
        //!          existant) white piece on the given square to the white king?
        bool piece_is_pinned(Square square) const noexcept;

        //! \brief Checks whether there are enough pieces on the board for any possible checkmate.
        //!
        //! The following piece sets on the board make checkmate possible:
        //! - Any single pawn, rook, or queen,
        //! - At least two bishops (of any color) on oppositely colored squares,
        //! - A bishop and knight (of an piece color or square color combination),
        //! - Two knights (of any piece color or square color combination).
        //! \param color The player color of the pieces to check.
        //! \returns If there are enough pieces on the board on one side to make a checkmate arrangement.
        bool enough_material_to_checkmate(Piece_Color color) const noexcept;

        //! \brief Checks whether there are enough pieces on the board for any possible checkmate.
        //!
        //! The following piece sets on the board make checkmate possible:
        //! - Any single pawn, rook, or queen,
        //! - At least two bishops (of any color) on oppositely colored squares,
        //! - A bishop and knight (of an piece color or square color combination),
        //! - Two knights (of any piece color or square color combination).
        //! \returns If there are enough pieces on the board to make a checkmate arrangement.
        //!          If the method returns false, this will usually lead to a drawn game.
        bool enough_material_to_checkmate() const noexcept;

        //! \brief Determines whether a move will capture on the current board.
        //!
        //! \param move Move to check.
        //! \returns If the move captures an opponent's piece.
        //! \exception assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
        bool move_captures(const Move& move) const noexcept;

        //! \brief Determines whether a move will change the material on the current board.
        //!
        //! \param move Move to check.
        //! \returns If the move captures an opponent's piece or promotes a pawn.
        //! \exception assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
        bool move_changes_material(const Move& move) const noexcept;

        //! \brief Number of moves since the last pawn move or capturing move.
        //!
        //! If this count reaches 100 (50 moves on each side), the game ends in
        //! a draw.
        size_t moves_since_pawn_or_capture() const noexcept;

        //! \brief Counts the number of times the current board position has been seen in the last few plies.
        //!
        //! \param depth The number of plies to look back for equivalent positions.
        std::ptrdiff_t repeat_count_from_depth(size_t depth) const noexcept;

        //! \brief Returns the Zobrist hash of the current state of the board.
        //!
        //! See https://en.wikipedia.org/wiki/Zobrist_hashing for details.
        uint64_t board_hash() const noexcept;

        //! \brief Returns whether the specified player has castled.
        //!
        //! \param player The color of the player being queried.
        bool player_castled(Piece_Color player) const noexcept;

        //! \brief Gets the ply move during which a player castled.
        //!
        //! \param player The color of the player being queried.
        //! \returns The ply count when the player castled, or MAX(size_t) if the player has not castled.
        size_t castling_move_index(Piece_Color player) const noexcept;

        //! \brief Create a copy of the board with a random pawn removed.
        //!
        //! This can be used by a Player to calibrate the value of a centipawn.
        //! \exception Debug assertion failure if there are no pawns on the board.
        Board without_random_pawn() const noexcept;

        //! \brief Returns a list of moves that results in a quiescent version of the board.
        //!
        //! This list is created by performing all possible captures on
        //! the ending square of the last move. If multiple captures are
        //! possible, the weakest pieces goes first according to the piece
        //! values array.
        //! \param piece_values An array indexed by Piece::type() that gives
        //!        the value of the piece.
        std::vector<const Move*> quiescent(const std::array<double, 6>& piece_values) const noexcept;

        //! \brief Returns the number of moves available to the opponent prior to the opponent's last move.
        //!
        //! Returns 0 if no moves have been made on the board.
        size_t previous_moves_count() const noexcept;

    private:
        std::array<Piece, 64> board;
        Fixed_Capacity_Vector<uint64_t, 101> repeat_count;
        Piece_Color turn_color = Piece_Color::WHITE;
        size_t game_move_count = 0;
        const Move* previous_move = nullptr;
        std::array<bool, 64> unmoved_positions{};
        Square en_passant_target;
        uint64_t starting_hash{};
        std::array<Square, 2> king_location;
        Square checking_square;
        size_t plies_at_construction;

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
        const std::bitset<16>& moves_attacking_square(Square square, Piece_Color attacking_color) const noexcept;
        const std::bitset<16>& checking_moves() const noexcept;
        Square find_checking_square() const noexcept;

        // Information cache for gene reference
        std::array<size_t, 2> castling_index{size_t(-1), size_t(-1)};
        std::array<int, 2> castling_movement{0, 0};

        // Caches
        std::vector<const Move*> legal_moves_cache;
        size_t prior_moves_count = 0;

        void recreate_move_caches() noexcept;

        Piece& piece_on_square(Square square) noexcept;
        void remove_piece(Square square) noexcept;
        void move_piece(const Move& move) noexcept;
        bool no_legal_moves() const noexcept;
        void make_en_passant_targetable(Square square) noexcept;
        void clear_en_passant_target() noexcept;
        bool is_en_passant_targetable(Square square) const noexcept;
        bool is_in_legal_moves_list(const Move& move) const noexcept;
        void place_piece(Piece piece, Square square) noexcept;
        bool all_empty_between(Square start, Square end) const noexcept;
        void set_already_moved(Square square, bool piece_has_already_moved) noexcept;
        void update_board(const Move& move) noexcept;
        void switch_turn() noexcept;
        Game_Result move_result() const noexcept;

        // Track threefold repetition and fifty-move rule
        void add_board_position_to_repeat_record() noexcept;
        void add_to_repeat_count(uint64_t new_hash) noexcept;
        ptrdiff_t current_board_position_repeat_count() const noexcept;
        void clear_repeat_count() noexcept;

        // Zobrist hashing (implementation of threefold/fifty-move tracking)
        uint64_t current_board_hash = 0;

        void update_board_hash(Square square) noexcept;
        uint64_t square_hash(Square square) const noexcept;
        void update_whose_turn_hash() noexcept;

        bool king_multiply_checked() const noexcept;

        static void fen_parse_assert(bool assertion, const std::string& input_fen, const std::string& failure_message);

        // Moves with side effects are friends of Board
        friend class Castle; // moves second piece
        friend class En_Passant; // capture piece on another square
        friend class Pawn_Promotion; // replace piece
        friend class Pawn_Double_Move; // mark square as En Passant target
        friend class Pawn_Move; // reset three-fold and 50-move counts
};

#endif // BOARD_H
