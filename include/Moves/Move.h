#ifndef MOVE_H
#define MOVE_H

#include <string>

#include "Game/Square.h"

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

        virtual ~Move() = default;

        //! \brief This saves work by preventing all unnecessary copying (which is all copying).
        Move(const Move&) = delete;

        //! \brief Since there's only one instance of every Move, assignment can only lose information.
        Move& operator=(const Move&) = delete;

        //! \brief Further modifies the state of the board.
        //!
        //! Side effects are changes to the state of the board beyond the change
        //! in position of the moved piece and captured piece (movement by rook
        //! in castling, marking a square as a en passant target after a double
        //! pawn move, etc.).
        //!
        //! The default move has no side effects.
        //! \param board The board upon which the side effects are applied.
        virtual void side_effects(Board& board) const noexcept;

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
        bool is_en_passant() const noexcept;

        //! \brief Returns the piece that a pawn will be promoted to, if applicable.
        virtual Piece promotion() const noexcept;

        //! \brief Returns the symbol representing the promoted piece if this move is a pawn promotion type. All other moves return '\\0'.
        //!
        //! \returns the PGN symbol of the promotion piece, if any.
        virtual char promotion_piece_symbol() const noexcept;

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

    protected:
        //! \brief Change the ability of this Move to capture.
        //!
        //! \param capturing_ability Whether this move should be able to capture.
        void set_capturing_ability(bool capturing_ability) noexcept;

        //! \brief Indicate that the Move being created is an en passant capture.
        void mark_as_en_passant() noexcept;

        //! \brief A textual representation of a move in PGN format without consequences ('+' for check, etc.).
        //!
        //! \param board The board on which the move is about to be made.
        //! \returns The movement portion of a PGN move entry.
        virtual std::string algebraic_base(const Board& board) const noexcept;

        //! \brief How far move travels horizontally.
        //!
        //! \returns The distance in squares between the start and end files.
        int file_change() const noexcept;

        //! \brief How far move travels vertically.
        //!
        //! \returns The distance in squares between the start and end ranks.
        int rank_change() const noexcept;

    private:
        Square origin;
        Square destination;

        bool able_to_capture = true;
        bool is_en_passant_move = false;

        virtual bool move_specific_legal(const Board& board) const noexcept;
        std::string result_mark(Board board) const noexcept;
};

#endif // MOVE_H
