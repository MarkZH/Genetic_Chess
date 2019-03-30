#include "Moves/Move.h"

#include <cassert>
#include <cmath>
#include <cctype>
#include <stdexcept>

#include "Game/Board.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"


//! Constructs a move with no special rules.

//! \param file_start File of square where move starts.
//! \param rank_start Rank of square where the move starts.
//! \param file_end File of square where move ends.
//! \param rank_end Rank of square where the move ends.
Move::Move(char file_start, int rank_start,
           char file_end,   int rank_end) :
               starting_file(file_start),
               starting_rank(rank_start),
               ending_file(file_end),
               ending_rank(rank_end),
               able_to_capture(true),
               is_en_passant_move(false),
               is_castling_move(false)
{
    if( ! Board::inside_board(starting_file, starting_rank))
    {
        throw std::runtime_error(std::string("Invalid starting square: ") + starting_file + std::to_string(starting_rank));
    }

    if( ! Board::inside_board(ending_file, ending_rank))
    {
        throw std::runtime_error(std::string("Invalid ending square: ") + ending_file + std::to_string(ending_rank));
    }

    if(file_change() == 0 && rank_change() == 0)
    {
        throw std::runtime_error(std::string("Zero-distance moves are illegal: ")
                                 + starting_file + std::to_string(starting_rank)
                                 + " --> "
                                 + ending_file + std::to_string(ending_rank));
    }
}

//! Further modifies the state of the board.

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

//! This method checks for attacking a piece of the same color,
//! attacking a piece when the move cannot capture, special rules,
//! and whether the king is in check after the move. It does not
//! check if there are intervening pieces (see Board::recreate_move_cache()).
//! \param board The board on which the Move's legality is tested.
bool Move::is_legal(const Board& board) const
{
#ifndef NDEBUG
    auto moving_piece = board.piece_on_square(starting_file, starting_rank);
#endif
    assert(moving_piece);
    assert(moving_piece->color() == board.whose_turn());
    assert(moving_piece->can_move(this));

    auto attacked_piece = board.piece_on_square(ending_file, ending_rank);
    if(attacked_piece)
    {
        if( ! can_capture() || board.whose_turn() == attacked_piece->color())
        {
            return false;
        }
    }

    return move_specific_legal(board) && ! board.king_is_in_check_after_move(*this);
}

//! Returns whether a move is legal according to rules not covered by Move::is_legal().

//! This method is overridden by subclassed moves with
//! special rules. The standard Move just returns true;
//! \param board The board on which legality is being checked.
bool Move::move_specific_legal(const Board&) const
{
    return true;
}

//! Check whether this move can land on an opponent-occupied square.

//! \returns Whether this move is allowed to capture.
bool Move::can_capture() const
{
    return able_to_capture;
}

//! File of square where move starts.

//! \returns The letter label of the starting square.
char Move::start_file() const
{
    return starting_file;
}

//! Rank of square where move starts.

//! \returns The numerical label of the starting square.
int Move::start_rank() const
{
    return starting_rank;
}

//! How far move travels horizontally.

//! \returns The distance in squares between the start and end files.
int Move::file_change() const
{
    return ending_file - starting_file;
}

//! How far move travels vertically.

//! \returns The distance in squares between the start and end ranks.
int Move::rank_change() const
{
    return ending_rank - starting_rank;
}

//! File of square where move ends.

//! \returns The letter label of the ending square.
char Move::end_file() const
{
    return ending_file;
}

//! Rank of square where move ends.

//! \returns The numerical label of the ending square.
int Move::end_rank() const
{
    return ending_rank;
}

//! Creates a textual representation of a move suitable for a PGN game record.

//! \param board A Board instance just prior to the move being made.
//! \returns The full PGN record of a move.
std::string Move::game_record_item(const Board& board) const
{
    return game_record_move_item(board) + game_record_ending_item(board);
}

//! A textual representation of a move in PGN format without consequences ('+' for check, etc.).

//! \param board The board on which the move is about to be made.
//! \returns The movement portion of a PGN move entry.
std::string Move::game_record_move_item(const Board& board) const
{
    auto original_piece = board.piece_on_square(starting_file, starting_rank);
    std::string move_record = original_piece->pgn_symbol();

    bool record_file = false;
    bool record_rank = false;
    for(char file_other = 'a'; file_other <= 'h'; ++file_other)
    {
        for(int rank_other = 1; rank_other <= 8; ++rank_other)
        {
            if( ! board.piece_on_square(file_other, rank_other))
            {
                continue;
            }
            if(file_other == starting_file && rank_other == starting_rank)
            {
                continue;
            }
            if(file_other == ending_file && rank_other == ending_rank)
            {
                continue;
            }
            auto new_piece = board.piece_on_square(file_other, rank_other);
            if(original_piece != new_piece)
            {
                continue;
            }

            if(board.is_legal(file_other, rank_other, ending_file, ending_rank))
            {
                if(file_other != starting_file && ! record_file)
                {
                    record_file = true;
                    continue;
                }
                if(rank_other != starting_rank)
                {
                    record_rank = true;
                }
            }
        }
    }

    if(record_file)
    {
        move_record += starting_file;
    }
    if(record_rank)
    {
        move_record += std::to_string(starting_rank);
    }

    if(board.piece_on_square(ending_file, ending_rank))
    {
        move_record += 'x';
    }

    move_record += ending_file + std::to_string(ending_rank);

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

//! The first two characters indicate the starting square, the next two
//! indicate the ending square, and a final optional character to indicate
//! a pawn promtion.
std::string Move::coordinate_move() const
{
    auto result = starting_file
                  + std::to_string(starting_rank)
                  + ending_file
                  + std::to_string(ending_rank);

    if(promotion_piece_symbol())
    {
        result += char(std::tolower(promotion_piece_symbol()));
    }

    return result;
}

//! Indicates whether this move is en passant, which needs special handling elsewhere.

//! \returns Whether this is an instance of the En_Passant class.
bool Move::is_en_passant() const
{
    return is_en_passant_move;
}

//! Indicates whether this move is a castling move, a fact which needs special handling elsewhere.

//! \returns Whether this is an instance of the Castle class.
bool Move::is_castling() const
{
    return is_castling_move;
}

//! Returns the symbol representing the promoted piece if this move is a pawn promotion type. All other moves return '\0'.

//! \returns the PGN symbol of the promotion piece, if any.
char Move::promotion_piece_symbol() const
{
    return '\0';
}
