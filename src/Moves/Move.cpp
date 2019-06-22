#include "Moves/Move.h"

#include <cassert>
#include <cmath>
#include <cctype>
#include <string>
#include <stdexcept>

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"


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

void Move::side_effects(Board&) const
{
}

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

bool Move::move_specific_legal(const Board&) const
{
    return true;
}

bool Move::can_capture() const
{
    return able_to_capture;
}

Square Move::start() const
{
    return origin;
}

Square Move::end() const
{
    return destination;
}

Square_Difference Move::movement() const
{
    return end() - start();
}

int Move::file_change() const
{
    return end().file() - start().file();
}

int Move::rank_change() const
{
    return end().rank() - start().rank();
}

std::string Move::game_record_item(const Board& board) const
{
    return game_record_move_item(board) + game_record_ending_item(board);
}

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
    return ((board.king_is_in_check() && ! result.game_has_ended()) ? "+" : "") + result.game_record_annotation();
}

std::string Move::coordinate_move() const
{
    auto result = start().string() + end().string();

    if(promotion_piece_symbol())
    {
        result += char(std::tolower(promotion_piece_symbol()));
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

void Move::adjust_end_file(int adjust)
{
    destination += Square_Difference{adjust, 0};
}

void Move::adjust_end_rank(int adjust)
{
    destination += Square_Difference{0, adjust};
}

size_t Move::attack_index() const
{
    return attack_index(movement());
}

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

Square_Difference Move::attack_direction_from_index(size_t index)
{
    if(index & 8)
    {
        // Knight move
        auto file_direction = (index & 4) ? 1 : -1;
        auto rank_direction = (index & 2) ? 1 : -1;
        auto file_step = (index & 1) ? 2 : 1;
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
