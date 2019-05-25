#include "Moves/Move.h"

#include <cassert>
#include <cmath>
#include <cctype>
#include <stdexcept>

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"


//! Constructs a move with no special rules.
//
//! \param start The Square where move starts.
//! \param end   The Square where move ends.
Move::Move(Square start, Square end) :
               able_to_capture(true),
               is_en_passant_move(false),
               is_castling_move(false),
               origin(start),
               destination(end)
{
    if( ! start.inside_board())
    {
        throw std::invalid_argument(std::string("Invalid starting square: ") + start.string());
    }

    if( ! end.inside_board())
    {
        throw std::invalid_argument(std::string("Invalid ending square: ") + end.string());
    }

    if(file_change() == 0 && rank_change() == 0)
    {
        throw std::invalid_argument(std::string("Zero-distance moves are illegal: ")
                                    + start.string() + " --> " + end.string());
    }
}

//! Further modifies the state of the board.
//
//! Side effects are changes to the state of the board beyond the change
//! in position of the moved piece and captured piece (movement by rook
//! in castling, marking a square as a en passant target after a double
//! pawn move, etc.).
//!
//! The default move has no side effects.
//! \param board The board upon which the side effects are applied.
void Move::side_effects(Board&) const
{
}

//! Checks if a move is legal on a given Board.
//
//! This method checks for attacking a piece of the same color,
//! attacking a piece when the move cannot capture, special rules,
//! and whether the king is in check after the move. It does not
//! check if there are intervening pieces (see Board::recreate_move_cache()).
//! \param board The board on which the Move's legality is tested.
bool Move::is_legal(const Board& board) const
{
#ifndef NDEBUG
    auto moving_piece = board.piece_on_square(start());
#endif
    assert(moving_piece);
    assert(moving_piece.color() == board.whose_turn());
    assert(moving_piece.can_move(this));

    auto attacked_piece = board.piece_on_square(end());
    if(attacked_piece)
    {
        if( ! can_capture() || board.whose_turn() == attacked_piece.color())
        {
            return false;
        }
    }

    return move_specific_legal(board) && ! board.king_is_in_check_after_move(*this);
}

//! Returns whether a move is legal according to rules not covered by Move::is_legal().
//
//! This method is overridden by subclassed moves with
//! special rules. The standard Move just returns true;
//! \param board The board on which legality is being checked.
bool Move::move_specific_legal(const Board&) const
{
    return true;
}

//! Check whether this move can land on an opponent-occupied square.
//
//! \returns Whether this move is allowed to capture.
bool Move::can_capture() const
{
    return able_to_capture;
}

//! The Square the Move originates from.
Square Move::start() const
{
    return origin;
}

//! The Square the Move ends on.
Square Move::end() const
{
    return destination;
}

//! The total movement of a move.
//
//! \returns A pair of integers indicating the two-dimensional movement.
//!          Equivalent to std::make_pair(file_change(), rank_change()).
Square_Difference Move::movement() const
{
    return end() - start();
}
//! How far move travels horizontally.
//
//! \returns The distance in squares between the start and end files.
int Move::file_change() const
{
    return end().file() - start().file();
}

//! How far move travels vertically.
//
//! \returns The distance in squares between the start and end ranks.
int Move::rank_change() const
{
    return end().rank() - start().rank();
}

//! Creates a textual representation of a move suitable for a PGN game record.
//
//! \param board A Board instance just prior to the move being made.
//! \returns The full PGN record of a move.
std::string Move::game_record_item(const Board& board) const
{
    return game_record_move_item(board) + game_record_ending_item(board);
}

//! A textual representation of a move in PGN format without consequences ('+' for check, etc.).
//
//! \param board The board on which the move is about to be made.
//! \returns The movement portion of a PGN move entry.
std::string Move::game_record_move_item(const Board& board) const
{
    auto original_piece = board.piece_on_square(start());
    std::string move_record = original_piece.pgn_symbol();

    bool record_file = false;
    bool record_rank = false;
    for(auto other_square : Square::all_squares())
    {
        if( ! board.piece_on_square(other_square))
        {
            continue;
        }
        if(other_square == start() || other_square == end())
        {
            continue;
        }

        auto new_piece = board.piece_on_square(other_square);
        if(original_piece != new_piece)
        {
            continue;
        }

        if(board.is_legal(other_square, end()))
        {
            if(other_square.file() != start().file() && ! record_file)
            {
                record_file = true;
                continue;
            }
            if(other_square.rank() != start().rank())
            {
                record_rank = true;
            }
        }
    }

    if(record_file)
    {
        move_record += start().file();
    }
    if(record_rank)
    {
        move_record += std::to_string(start().rank());
    }

    if(board.piece_on_square(end()))
    {
        move_record += 'x';
    }

    move_record += end().string();

    return move_record;
}

std::string Move::game_record_ending_item(Board board) const
{
    auto result = board.submit_move(*this);
    std::string appendage;

    if(board.king_is_in_check() && result.winner() == NONE)
    {
        appendage.push_back('+');
    }

    return appendage + result.game_record_annotation();
}

//! Returns a textual representation of a move in coordinate notation.
//
//! The first two characters indicate the starting square, the next two
//! indicate the ending square, and a final optional character to indicate
//! a pawn promtion.
std::string Move::coordinate_move() const
{
    auto result = start().string() + end().string();

    if(promotion_piece_symbol())
    {
        result += char(std::tolower(promotion_piece_symbol()));
    }

    return result;
}

//! Indicates whether this move is en passant, which needs special handling elsewhere.
//
//! \returns Whether this is an instance of the En_Passant class.
bool Move::is_en_passant() const
{
    return is_en_passant_move;
}

//! Indicates whether this move is a castling move, a fact which needs special handling elsewhere.
//
//! \returns Whether this is an instance of the Castle class.
bool Move::is_castling() const
{
    return is_castling_move;
}

//! Returns the symbol representing the promoted piece if this move is a pawn promotion type. All other moves return '\0'.
//
//! \returns the PGN symbol of the promotion piece, if any.
char Move::promotion_piece_symbol() const
{
    return '\0';
}

//! Adjust the file of the square a move ends on.
//
//! This is used for Pawn_Move derivitives since that constructor
//! forces single moves.
//
//! \param adjust The size of the adjustment.
void Move::adjust_end_file(int adjust)
{
    destination += Square_Difference{adjust, 0};
}

//! Adjust the rank of the square a move ends on.
//
//! This is used for Pawn_Move derivitives since that constructor
//! forces single moves.
//
//! \param adjust The size of the adjustment.
void Move::adjust_end_rank(int adjust)
{
    destination += Square_Difference{0, adjust};
}

//! Assigns a unique index to the direction of movement of a possibly capturing move.
//
//! \returns An unsigned integer in the range [0,15] corresponding to one of
//!          2 horizontal moves, 2 vertical moves, 4 diagonal moves, and
//!          8 knight moves.
size_t Move::attack_index() const
{
    return attack_index(movement());
}

//! Returns a unique move direction index for a manually specified move. See Move::attack_index().
//
//! \param move The difference between two Squares.
//! \returns The same result as a call to Move::attack_index() with the same file_change() and rank_change().
size_t Move::attack_index(const Square_Difference& move)
{
    size_t result = 0;

    // First bit == knight move or not
    if(std::abs(move.rank_change*move.file_change) == 2) // 1x2 or 2x1
    {
        result += 8;
        result += 4*(move.file_change > 0);
        result += 2*(move.rank_change > 0);
        result += 1*(std::abs(move.file_change) > std::abs(move.rank_change));
    }
    else
    {
        // Second bit == rook move or not
        if(move.rank_change == 0 || move.file_change == 0)
        {
            result += 4;
            result += 2*(move.rank_change != 0);
            result += 1*(move.rank_change + move.file_change > 0);
        }
        else
        {
            // Bishop moves
            result += 2*(move.file_change > 0);
            result += 1*(move.rank_change > 0);
        }
    }

    return result;
}

//! Returns the movement corresponding to an index given by Move::attack_index().
//
//! \returns A pair of integers giving the direction of an attacking move.
Square_Difference Move::attack_direction_from_index(size_t index)
{
    if(index & 8)
    {
        // Knight move
        auto file_direction = (index & 4) ? 1 : -1;
        auto rank_direction = (index & 2) ? 1 : -1;
        auto file_step =( index & 1) ? 2 : 1;
        auto rank_step = 3 - file_step;
        return {file_step*file_direction, rank_step*rank_direction};
    }
    else
    {
        if(index & 4)
        {
            // Rook move
            if(index & 2)
            {
                // Vertical move
                return {0, (index & 1) ? 1 : -1};
            }
            else
            {
                // Horizontal move
                return {(index & 1) ? 1 : -1, 0};
            }
        }
        else
        {
            // Bishop move
            return {(index & 2) ? 1 : -1, (index & 1) ? 1 : -1};
        }
    }
}
