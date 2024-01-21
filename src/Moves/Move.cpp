#include "Moves/Move.h"

#include <cassert>
#include <cctype>
#include <string>

#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"
#include "Moves/Direction.h"

#include "Utility/String.h"


Move::Move(const Square start, const Square end) noexcept : origin(start), destination(end)
{
    assert(start.inside_board());
    assert(end.inside_board());
    assert(start != end);
}

Move Move::pawn_move(Square start, Piece_Color pawn_color, Piece promote) noexcept
{
    const auto end = start + Square_Difference{0, pawn_color == Piece_Color::WHITE ? 1 : -1};
    auto move = Move(start, end);
    move.set_capturing_ability(false);
    move.setup_pawn_promotion(pawn_color, promote);
    return move;
}

Move Move::pawn_capture(Square start, Direction direction, Piece_Color pawn_color, Piece promote) noexcept
{
    const auto end = start + Square_Difference{direction == Direction::LEFT ? -1 : 1, pawn_color == Piece_Color::WHITE ? 1 : -1 };
    auto move = Move(start, end);
    move.set_capturing_ability(true);
    move.setup_pawn_promotion(pawn_color, promote);
    return move;
}

Move Move::pawn_double_move(Piece_Color pawn_color, char file) noexcept
{
    const auto start = Square{file, pawn_color == Piece_Color::WHITE ? 2 : 7};
    const auto end = start + Square_Difference{0, pawn_color == Piece_Color::WHITE ? 2 : -2};
    auto move = Move(start, end);
    move.set_capturing_ability(false);
    return move;
}

void Move::side_effects(Board& board) const noexcept
{
    if(board.piece_on_square(end()).type() == Piece_Type::PAWN)
    {
        if(std::abs(rank_change()) == 2)
        {
            board.make_en_passant_targetable(start() + Square_Difference{0, rank_change()/2});
        }
        else if(end().rank() == 1 || end().rank() == 8)
        {
            board.place_piece(promotion(), end());
        }
    }
}

bool Move::is_legal(const Board& board) const noexcept
{
    assert(board.piece_on_square(start()));
    assert(board.piece_on_square(start()).color() == board.whose_turn());
    assert(board.piece_on_square(start()).can_move(this));

    if(const auto attacked_piece = board.piece_on_square(end()))
    {
        if( ! can_capture() || board.whose_turn() == attacked_piece.color())
        {
            return false;
        }
    }

    return move_specific_legal(board) && ! board.king_is_in_check_after_move(*this);
}

bool Move::move_specific_legal(const Board& board) const noexcept
{
    if(board.piece_on_square(start()).type() == Piece_Type::PAWN)
    {
        return (bool(board.piece_on_square(end())) == can_capture()) || is_en_passant(board);
    }
    else
    {
        return true;
    }
}

bool Move::can_capture() const noexcept
{
    return able_to_capture;
}

Square Move::start() const noexcept
{
    return origin;
}

Square Move::end() const noexcept
{
    return destination;
}

Square_Difference Move::movement() const noexcept
{
    return end() - start();
}

int Move::file_change() const noexcept
{
    return end().file() - start().file();
}

int Move::rank_change() const noexcept
{
    return end().rank() - start().rank();
}

std::string Move::algebraic(const Board& board) const noexcept
{
    return algebraic_base(board) + result_mark(board);
}

std::string Move::algebraic_base(const Board& board) const noexcept
{
    const auto original_piece = board.piece_on_square(start());

    auto record_file = original_piece.type() == Piece_Type::PAWN && board.move_captures(*this);
    auto record_rank = false;
    for(auto other_move : board.legal_moves())
    {
        if(other_move->start() == start())
        {
            continue;
        }

        const auto new_piece = board.piece_on_square(other_move->start());
        if(original_piece == new_piece && end() == other_move->end())
        {
            if(other_move->start().file() != start().file() && ! record_file)
            {
                record_file = true;
            }
            else if(other_move->start().rank() != start().rank())
            {
                record_rank = true;
            }
        }
    }

    auto move_record = original_piece.pgn_symbol();
    if(record_file)                { move_record += start().file(); }
    if(record_rank)                { move_record += std::to_string(start().rank()); }
    if(board.move_captures(*this)) { move_record += 'x'; }
    move_record += end().text();
    if(promotion())                { move_record += std::string("=") + promotion_piece_symbol(); }
    return move_record;
}

std::string Move::result_mark(Board board) const noexcept
{
    const auto result = board.play_move(*this);
    if(board.king_is_in_check())
    {
        if(result.winner() == Winner_Color::NONE)
        {
            return "+";
        }
        else
        {
            return "#";
        }
    }
    else
    {
        return {};
    }
}

void Move::setup_pawn_promotion([[maybe_unused]] Piece_Color pawn_color, Piece promote) noexcept
{
    assert( ! promote || start().rank() == (pawn_color == Piece_Color::WHITE ? 7 : 2));
    assert( ! promote || pawn_color == promote.color());
    pawn_promotion = promote;
}

std::string Move::coordinates() const noexcept
{
    const auto result = start().text() + end().text();
    if(promotion_piece_symbol())
    {
        return result + String::tolower(promotion_piece_symbol());
    }
    else
    {
        return result;
    }
}

bool Move::is_en_passant(const Board& board) const noexcept
{
    return board.piece_on_square(start()).type() == Piece_Type::PAWN && board.en_passant_target == end();
}

bool Move::is_castle(const Board& board) const noexcept
{
    return board.piece_on_square(start()).type() == Piece_Type::KING && std::abs(file_change()) == 2;
}

Piece Move::promotion() const noexcept
{
    return pawn_promotion;
}

char Move::promotion_piece_symbol() const noexcept
{
    return promotion() ? promotion().pgn_symbol().front() : '\0';
}

size_t Move::attack_index() const noexcept
{
    return attack_index(movement());
}

size_t Move::attack_index(const Square_Difference& move) noexcept
{
    constexpr auto xx = size_t(-1); // indicates invalid moves and should never be returned
    constexpr size_t array_width = 15;
    //                                                      file change
    //                                   -7, -6, -5, -4, -3, -2, -1,  0,  1,  2,  3,  4,  5,  6,  7
    static constexpr size_t indices[] = { 0, xx, xx, xx, xx, xx, xx,  1, xx, xx, xx, xx, xx, xx,  2,  //  7
                                         xx,  0, xx, xx, xx, xx, xx,  1, xx, xx, xx, xx, xx,  2, xx,  //  6
                                         xx, xx,  0, xx, xx, xx, xx,  1, xx, xx, xx, xx,  2, xx, xx,  //  5
                                         xx, xx, xx,  0, xx, xx, xx,  1, xx, xx, xx,  2, xx, xx, xx,  //  4
                                         xx, xx, xx, xx,  0, xx, xx,  1, xx, xx,  2, xx, xx, xx, xx,  //  3
                                         xx, xx, xx, xx, xx,  0, 15,  1,  8,  2, xx, xx, xx, xx, xx,  //  2
                                         xx, xx, xx, xx, xx, 14,  0,  1,  2,  9, xx, xx, xx, xx, xx,  //  1
                                          3,  3,  3,  3,  3,  3,  3, xx,  4,  4,  4,  4,  4,  4,  4,  //  0  rank change
                                         xx, xx, xx, xx, xx, 13,  5,  6,  7, 10, xx, xx, xx, xx, xx,  // -1
                                         xx, xx, xx, xx, xx,  5, 12,  6, 11,  7, xx, xx, xx, xx, xx,  // -2
                                         xx, xx, xx, xx,  5, xx, xx,  6, xx, xx,  7, xx, xx, xx, xx,  // -3
                                         xx, xx, xx,  5, xx, xx, xx,  6, xx, xx, xx,  7, xx, xx, xx,  // -4
                                         xx, xx,  5, xx, xx, xx, xx,  6, xx, xx, xx, xx,  7, xx, xx,  // -5
                                         xx,  5, xx, xx, xx, xx, xx,  6, xx, xx, xx, xx, xx,  7, xx,  // -6
                                          5, xx, xx, xx, xx, xx, xx,  6, xx, xx, xx, xx, xx, xx,  7}; // -7

    assert(-7 <= move.rank_change && move.rank_change <= 7);
    assert(-7 <= move.file_change && move.file_change <= 7);

    const auto i = array_width*(7 - move.rank_change) + (move.file_change + 7);

    assert(indices[i] < 16);
    return indices[i];
}

Square_Difference Move::attack_direction_from_index(const size_t index) noexcept
{
    //                     index: 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
    static constexpr int dx[] = {-1,  0,  1, -1,  1, -1,  0,  1,  1,  2,  2,  1, -1, -2, -2, -1};
    static constexpr int dy[] = { 1,  1,  1,  0,  0, -1, -1, -1,  2,  1, -1, -2, -2, -1,  1,  2};
    return {dx[index], dy[index]};
}

void Move::set_capturing_ability(const bool capturing_ability) noexcept
{
    able_to_capture = capturing_ability;
}
