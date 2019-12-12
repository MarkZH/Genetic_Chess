#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "Game/Square.h"

class Board;

//! A class to represent the movement of pieces.
class Move
{
    public:
        //! Constructs a move with no special rules.
        //
        //! \param start The Square where move starts.
        //! \param end   The Square where move ends.
        Move(Square start, Square end) noexcept;

        virtual ~Move() = default;

        //! This saves work by preventing all unnecessary copying (which is all copying).
        Move(const Move&) = delete;

        //! Since there's only one instance of every Move, assignment can only lose information.
        Move& operator=(const Move&) = delete;

        //! Further modifies the state of the board.
        //
        //! Side effects are changes to the state of the board beyond the change
        //! in position of the moved piece and captured piece (movement by rook
        //! in castling, marking a square as a en passant target after a double
        //! pawn move, etc.).
        //!
        //! The default move has no side effects.
        //! \param board The board upon which the side effects are applied.
        virtual void side_effects(Board& board) const noexcept;

        //! Checks if a move is legal on a given Board.
        //
        //! This method checks for attacking a piece of the same color,
        //! attacking a piece when the move cannot capture, special rules,
        //! and whether the king is in check after the move. It does not
        //! check if there are intervening pieces (see Board::recreate_move_cache()).
        //! \param board The board on which the Move's legality is tested.
        bool is_legal(const Board& board) const noexcept;

        //! Check whether this move can land on an opponent-occupied square.
        //
        //! \returns Whether this move is allowed to capture.
        bool can_capture() const noexcept;

        //! The Square the Move originates from.
        Square start() const noexcept;

        //! The Square the Move ends on.
        Square end() const noexcept;

        //! The total movement of a move.
        //
        //! \returns A value indicating the two-dimensional movement.
        //!          Equivalent to Square_Difference(file_change(), rank_change()).
        Square_Difference movement() const noexcept;

        //! How far move travels horizontally.
        //
        //! \returns The distance in squares between the start and end files.
        int file_change() const noexcept;

        //! How far move travels vertically.
        //
        //! \returns The distance in squares between the start and end ranks.
        int rank_change() const noexcept;

        //! Creates a textual representation of a move suitable for a PGN game record.
        //
        //! \param board A Board instance just prior to the move being made.
        //! \returns The full PGN record of a move.
        std::string game_record_item(const Board& board) const noexcept;

        //! Returns a textual representation of a move in coordinate notation.
        //
        //! The first two characters indicate the starting square, the next two
        //! indicate the ending square, and a final optional character to indicate
        //! a pawn promtion.
        std::string coordinate_move() const noexcept;

        //! Indicates whether this move is en passant, which needs special handling elsewhere.
        //
        //! \returns Whether this is an instance of the En_Passant class.
        bool is_en_passant() const noexcept;

        //! Indicates whether this move is a castling move, a fact which needs special handling elsewhere.
        //
        //! \returns Whether this is an instance of the Castle class.
        bool is_castling() const noexcept;

        //! Returns the symbol representing the promoted piece if this move is a pawn promotion type. All other moves return '\0'.
        //
        //! \returns the PGN symbol of the promotion piece, if any.
        virtual char promotion_piece_symbol() const noexcept;

        //! Assigns a unique index to the direction of movement of a possibly capturing move.
        //
        //! \returns An unsigned integer in the range [0,15] corresponding to one of
        //!          2 horizontal moves, 2 vertical moves, 4 diagonal moves, and
        //!          8 knight moves.
        size_t attack_index() const noexcept;

        //! Returns a unique move direction index for a manually specified move. See Move::attack_index().
        //
        //! \param move The difference between two Squares.
        //! \returns The same result as a call to Move::attack_index() with the same file_change() and rank_change().
        static size_t attack_index(const Square_Difference& move) noexcept;

        //! Returns the movement corresponding to an index given by Move::attack_index().
        //
        //! \returns A pair of integers giving the direction of an attacking move.
        static Square_Difference attack_direction_from_index(size_t index) noexcept;

    protected:
        //! Remove the ability to capture from the Move.
        void disable_capturing() noexcept;

        //! Restore the ability to capture to the Move.
        void enable_capturing() noexcept;

        //! Indicate that the Move being created is an en passant capture.
        void mark_as_en_passant() noexcept;

        //! Indicate that the Move being created is a castling move.
        void mark_as_castling() noexcept;

        //! Adjust the file of the square a move ends on.
        //
        //! This is used for Pawn_Move derivitives since that constructor
        //! forces single moves.
        //
        //! \param adjust The size of the adjustment.
        void adjust_end_file(int adjust) noexcept;

        //! Adjust the rank of the square a move ends on.
        //
        //! This is used for Pawn_Move derivitives since that constructor
        //! forces single moves.
        //
        //! \param adjust The size of the adjustment.
        void adjust_end_rank(int adjust) noexcept;

    private:
        Square origin;
        Square destination;

        bool able_to_capture = true;
        bool is_en_passant_move = false;
        bool is_castling_move = false;

        virtual bool move_specific_legal(const Board& board) const noexcept;
        virtual std::string game_record_move_item(const Board& board) const noexcept;
        std::string game_record_ending_item(Board board) const noexcept;
};

#endif // MOVE_H
