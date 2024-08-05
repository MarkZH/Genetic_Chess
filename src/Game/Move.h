#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <functional>

#include "Game/Square.h"
#include "Game/Piece.h"

//! \file

//! \brief Indicates a horizontal direction on the chess board.
enum class Direction { LEFT, RIGHT };

class Board;
class Piece;

//! \brief A class to represent the movement of pieces.
class Move
{
    public:
        //! \brief Constructs a move with no special rules.
        //!
        //! \param start The Square where move starts.
        //! \param end   The Square where move ends.
        Move(Square start, Square end) noexcept;

        //! \brief Construct a non-capturing pawn move.
        //! 
        //! \param start The square the pawn is initially on.
        //! \param pawn_color The color of the pawn to determine which direction it goes.
        //! \param promote What piece to promote the pawn to if it reaches the back ranks. This may be an invalid piece if no promotion is intended.
        //! 
        //! \returns A move instance with proper pawn rules in place.
        static Move pawn_move(Square start, Piece_Color pawn_color, Piece promote) noexcept;

        //! \brief Construct a capturing pawn move.
        //! 
        //! \param start The square the pawn is initially on.
        //! \param direction Whether the pawn capture to the left or right (from white's perspective).
        //! \param pawn_color The color of the pawn to determine which direction it goes.
        //! \param promote What piece to promote the pawn to if it reaches the back ranks. This may be an invalid piece if no promotion is intended.
        //! 
        //! \returns A move instance with proper pawn rules in place.
        static Move pawn_capture(Square start, Direction direction, Piece_Color pawn_color, Piece promote) noexcept;

        //! \brief Construct a pawn double move.
        //! 
        //! \param pawn_color The color of the pawn to determine which direction it goes and which rank it starts on.
        //! \param file Which file the pawn starts on.
        //! 
        //! \returns A move instance with proper pawn rules in place.
        static Move pawn_double_move(Piece_Color pawn_color, char file) noexcept;

        //! \brief Construct castling move.
        //! 
        //! \param king_color The color of the castling king.
        //! \param direction To which side the castling goes: Direction::LEFT for queenside and Direction::RIGHT for kingside.
        //! 
        //! \returns A move instance with proper pawn rules in place.
        static Move castle(Piece_Color king_color, Direction direction) noexcept;

        //! \brief Further modifies the state of the board.
        //!
        //! Side effects are changes to the state of the board beyond the change
        //! in position of the moved piece and captured piece (movement by rook
        //! in castling, marking a square as a en passant target after a double
        //! pawn move, etc.).
        //!
        //! The default move has no side effects.
        //! \param board The board upon which the side effects are applied.
        void side_effects(Board& board) const noexcept;

        //! \brief Checks if a move is legal on a given Board.
        //!
        //! This method checks for attacking a piece of the same color,
        //! attacking a piece when the move cannot capture, special rules,
        //! and whether the king is in check after the move. It does not
        //! check if there are intervening pieces (see Board::recreate_move_cache()).
        //! \param board The board on which the Move's legality is tested.
        bool is_legal(const Board& board) const noexcept;

        //! \brief Check whether this move can land on an opponent-occupied square.
        //!
        //! \returns Whether this move is allowed to capture.
        bool can_capture() const noexcept;

        //! \brief The Square the Move originates from.
        Square start() const noexcept;

        //! \brief The Square the Move ends on.
        Square end() const noexcept;

        //! \brief The total movement of a move.
        //!
        //! \returns A value indicating the two-dimensional movement.
        //!          Equivalent to Square_Difference(file_change(), rank_change()).
        Square_Difference movement() const noexcept;

        //! \brief Creates a textual representation of a move suitable for a PGN game record.
        //!
        //! \param board A Board instance just prior to the move being made.
        //! \returns The full PGN record of a move.
        std::string algebraic(const Board& board) const noexcept;

        //! \brief Returns a textual representation of a move in coordinate notation.
        //!
        //! The first two characters indicate the starting square, the next two
        //! indicate the ending square, and a final optional character to indicate
        //! a pawn promtion.
        std::string coordinates() const noexcept;

        //! \brief Indicates whether this move is en passant, which needs special handling elsewhere.
        //!
        //! \returns Whether this is an instance of the En_Passant class.
        bool is_en_passant(const Board& board) const noexcept;

        //! \brief Indicates whether the move is a castling move.
        bool is_castle() const noexcept;

        //! \brief Returns the piece that a pawn will be promoted to, if applicable.
        Piece promotion() const noexcept;

        //! \brief Returns the symbol representing the promoted piece if this move is a pawn promotion type. All other moves return '\\0'.
        //!
        //! \returns the PGN symbol of the promotion piece, if any.
        char promotion_piece_symbol() const noexcept;

        //! \brief Assigns a unique index to the direction of movement of a possibly capturing move.
        //!
        //! \returns An unsigned integer in the range [0,15] corresponding to one of
        //!          2 horizontal moves, 2 vertical moves, 4 diagonal moves, and
        //!          8 knight moves.
        size_t attack_index() const noexcept;

        //! \brief Returns a unique move direction index for a manually specified move. See Move::attack_index().
        //!
        //! \param move The difference between two Squares.
        //! \returns The same result as a call to Move::attack_index() with the same file_change() and rank_change().
        static size_t attack_index(const Square_Difference& move) noexcept;

        //! \brief Returns the movement corresponding to an index given by Move::attack_index().
        //!
        //! \returns A pair of integers giving the direction of an attacking move.
        static Square_Difference attack_direction_from_index(size_t index) noexcept;

        //! \brief Default initializer for Fixed_Capacity_Vector
        Move() = default;

    private:
        //! \brief Change the ability of this Move to capture.
        //!
        //! \param capturing_ability Whether this move should be able to capture.
        void set_capturing_ability(bool capturing_ability) noexcept;

        //! \brief A textual representation of a move in PGN format without consequences ('+' for check, etc.).
        //!
        //! \param board The board on which the move is about to be made.
        //! \returns The movement portion of a PGN move entry.
        std::string algebraic_base(const Board& board) const noexcept;

        //! \brief How far move travels horizontally.
        //!
        //! \returns The distance in squares between the start and end files.
        int file_change() const noexcept;

        //! \brief How far move travels vertically.
        //!
        //! \returns The distance in squares between the start and end ranks.
        int rank_change() const noexcept;

        Square origin;
        Square destination;

        bool able_to_capture = true;
        bool is_castling = false;

        std::function<bool(const Board&)> extra_rule = [](const Board&) { return true; };
        std::function<void(Board&)> side_effect = [](const Board&) {};

        Piece pawn_promotion;

        bool move_specific_legal(const Board& board) const noexcept;
        std::string result_mark(Board board) const noexcept;
        void setup_pawn_promotion(Piece_Color pawn_color, Piece promote) noexcept;
        void setup_pawn_rules() noexcept;
        void setup_castling_rules(Direction direction) noexcept;

        friend bool operator==(const Move&, const Move&) noexcept;
};

//! \brief Equality operator for Moves
bool operator==(const Move& a, const Move& b) noexcept;
#endif // MOVE_H
