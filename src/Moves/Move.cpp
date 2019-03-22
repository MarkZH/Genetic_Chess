#include "Moves/Move.h"

#include <cassert>
#include <cmath>
#include <cctype>

#include "Game/Board.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"

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

#ifndef NDEBUG
    auto moving_piece = board.piece_on_square(starting_file, starting_rank);
#endif
    assert(moving_piece);
    assert(moving_piece->color() == board.whose_turn());
    assert(moving_piece->can_move(this));

    auto attacked_piece = board.piece_on_square(ending_file, ending_rank);
    if(attacked_piece)
    {
        if(board.whose_turn() == attacked_piece->color())
        {
            return false;
        }

        if( ! can_capture())
        {
            return false;
        }
    }

    if( ! move_specific_legal(board))
    {
        return false;
    }

    return ! board.king_is_in_check_after_move(*this);
}

bool Move::move_specific_legal(const Board&) const
{
    return true;
}

bool Move::can_capture() const
{
    return able_to_capture;
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
    std::string appendage;

    if(board.king_is_in_check() && result.winner() == NONE)
    {
        appendage.push_back('+');
    }

    return appendage + result.game_record_annotation();
}

std::string Move::coordinate_move() const
{
    auto p = char(std::tolower(promotion_piece_symbol()));
    auto result = starting_file
                  + std::to_string(starting_rank)
                  + ending_file
                  + std::to_string(ending_rank);
    if(p)
    {
        result += p;
    }

    return result;
}

bool Move::is_en_passant() const
{
    return is_en_passant_move;
}

bool Move::is_castling() const
{
    return is_castling_move;
}

char Move::promotion_piece_symbol() const
{
    return '\0';
}
