#include "Moves/Move.h"

#include <cassert>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Exceptions/Illegal_Move_Exception.h"
#include "Utility.h"


Move::Move(char file_start, int rank_start,
           char file_end,   int rank_end) :
               starting_file(file_start),
               starting_rank(rank_start),
               ending_file(file_end),
               ending_rank(rank_end)
{
    assert(std::abs(file_change()) < 8);
    assert(std::abs(rank_change()) < 8);
    assert(file_change() != 0 || rank_change() != 0);
}

void Move::side_effects(Board&) const
{
}

bool Move::is_legal(const Board& board) const
{
    assert(Board::inside_board(starting_file, starting_rank));
    assert(Board::inside_board(ending_file, ending_rank));

    // Piece-move compatibility
    auto moving_piece = board.piece_on_square(starting_file, starting_rank);
    assert(moving_piece);
    assert(moving_piece->color() == board.whose_turn());
    assert(moving_piece->can_move(this));

    auto attacked_piece = board.piece_on_square(ending_file, ending_rank);
    if(attacked_piece)
    {
        // Cannot capture piece of same color
        if(moving_piece->color() == attacked_piece->color())
        {
            return false;
        }

        // Enforce non-capturing moves
        if( ! can_capture())
        {
            return false;
        }
    }


    if( ! move_specific_legal(board))
    {
        return false;
    }

    // Check that there are no intervening pieces for straight-line moves
    if( ! moving_piece->is_knight())
    {
        if( ! board.all_empty_between(starting_file, starting_rank, ending_file, ending_rank))
        {
            return false;
        }
    }

    // King should not be in check after move
    return ! board.king_is_in_check_after_move(*this);
}

bool Move::move_specific_legal(const Board&) const
{
    return true;
}

bool Move::can_capture() const
{
    return true;
}

char Move::start_file() const
{
    return starting_file;
}

int Move::start_rank() const
{
    return starting_rank;
}

int Move::file_change() const
{
    return ending_file - starting_file;
}

int Move::rank_change() const
{
    return ending_rank - starting_rank;
}

char Move::end_file() const
{
    return ending_file;
}

int Move::end_rank() const
{
    return ending_rank;
}

std::string Move::game_record_item(const Board& board) const
{
    return game_record_move_item(board) + game_record_ending_item(board);
}

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
    if(result.game_has_ended())
    {
        return result.get_game_record_annotation();
    }

    if(board.king_is_in_check())
    {
        return "+";
    }

    return {};

}

std::string Move::coordinate_move() const
{
    return starting_file
           + std::to_string(starting_rank)
           + ending_file
           + std::to_string(ending_rank);
}

bool Move::is_en_passant() const
{
    return false;
}

char Move::promotion_piece_symbol() const
{
    return '\0';
}

const Piece* Move::promotion_piece() const
{
    return nullptr;
}
