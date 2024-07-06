#include "Game/Piece.h"

#include <cctype>
#include <algorithm>
#include <cassert>
#include <array>
#include <cctype>
#include <vector>
#include <stdexcept>

#include "Game/Square.h"
#include "Game/Color.h"
#include "Game/Move.h"

#include "Utility/Fixed_Capacity_Vector.h"
#include "Utility/String.h"

namespace
{
    const std::string pgn_symbols = "PRNBQK";
    const auto invalid_code =
        []()
        {
            auto max_code = Piece(pgn_symbols.front()).index();
            for(const auto symbol : pgn_symbols)
            {
                const auto max_piece_code = std::max(Piece(symbol).index(), Piece(String::tolower(symbol)).index());
                max_code = std::max(max_code, max_piece_code);
            }
            return max_code + 1;
        }();

    Piece_Type piece_type_from_char(const char pgn_symbol)
    {
        const auto i = pgn_symbols.find(String::toupper(pgn_symbol));
        if(i == std::string::npos)
        {
            throw std::invalid_argument(pgn_symbol + std::string{ " is not a valid piece PGN symbol." });
        }
        return static_cast<Piece_Type>(i);
    }

    using SQ = Square_Difference;
    static const auto bishop_moves = std::vector<SQ>({ SQ{-1, -1}, SQ{-1, 1}, SQ{1, -1}, SQ{1, 1} });
    static const auto rook_moves = std::vector<SQ>({ SQ{-1, 0}, SQ{0, -1}, SQ{1,  0}, SQ{0, 1} });
    static const auto queen_moves = [=]()
        {
            auto result = rook_moves;
            result.insert(result.end(), bishop_moves.begin(), bishop_moves.end());
            return result;
        }();

    std::vector<Square_Difference> get_sliding_moves(const Piece_Type type) noexcept
    {
        switch(type)
        {
            case Piece_Type::ROOK:
                return rook_moves;
            case Piece_Type::BISHOP:
                return bishop_moves;
            case Piece_Type::QUEEN:
                return queen_moves;
            default:
                return {};
        }

    }

    std::vector<Square_Difference> get_non_sliding_moves(const Piece_Type type) noexcept
    {
        using SQ = Square_Difference;
        switch(type)
        {
            case Piece_Type::KNIGHT:
                return {SQ{1, 2}, SQ{2, 1}, SQ{-1, 2}, SQ{2, -1}, SQ{1, -2}, SQ{-2, 1}, SQ{-1, -2}, SQ{-2, -1}};
            case Piece_Type::KING:
                return queen_moves;
            default:
                return {};
        }
    }

    std::array<std::vector<std::vector<Move>>, 64> all_pawn_moves(const Piece_Color pawn_color) noexcept
    {
        auto result = std::array<std::vector<std::vector<Move>>, 64>{};
        const auto base_rank = pawn_color == Piece_Color::WHITE ? 1 : 8;
        const auto first_rank = pawn_color == Piece_Color::WHITE ? 2 : 7;
        const auto last_rank = pawn_color == Piece_Color::WHITE ? 7 : 2;
        const auto far_rank = pawn_color == Piece_Color::WHITE ? 8 : 1;
        
        for(const auto square : Square::all_squares())
        {
            auto& move_list = result[square.index()];
            if(square.rank() == base_rank || square.rank() == far_rank)
            {
                continue;
            }
            else if(square.rank() == last_rank)
            {
                for(const auto promote : {Piece_Type::QUEEN, Piece_Type::ROOK, Piece_Type::KNIGHT, Piece_Type::BISHOP})
                {
                    const auto promotion = Piece{pawn_color, promote};
                    move_list.push_back({Move::pawn_move(square, pawn_color, promotion)});
                    for(const auto direction : {Direction::LEFT, Direction::RIGHT})
                    {
                        if((direction == Direction::LEFT && square.file() == 'a')
                           || (direction == Direction::RIGHT && square.file() == 'h'))
                        {
                            continue;
                        }
                        move_list.push_back({Move::pawn_capture(square, direction, pawn_color, promotion)});
                    }
                }
            }
            else
            {
                move_list.push_back({Move::pawn_move(square, pawn_color, {})});
                if(square.rank() == first_rank)
                {
                    move_list.back().push_back(Move::pawn_double_move(pawn_color, square.file()));
                }

                for(const auto direction : { Direction::LEFT, Direction::RIGHT })
                {
                    if((direction == Direction::LEFT && square.file() == 'a')
                       || (direction == Direction::RIGHT && square.file() == 'h'))
                    {
                        continue;
                    }
                    move_list.push_back({Move::pawn_capture(square, direction, pawn_color, {})});
                }
            }
        }

        return result;
    }

    std::array<std::vector<std::vector<Move>>, 64> get_special_moves(const Piece_Color color, const Piece_Type type) noexcept
    {
        if(type == Piece_Type::KING)
        {
            auto result = std::array<std::vector<std::vector<Move>>, 64>{};
            for(const auto direction : {Direction::LEFT, Direction::RIGHT})
            {
                const auto move = Move::castle(color, direction);
                result[move.start().index()].push_back({move});
            }
            return result;
        }
        else if(type == Piece_Type::PAWN)
        {
            return all_pawn_moves(color);

        }
        else
        {
            return {};
        }
    }
}

Piece::Piece() noexcept : piece_code(invalid_code)
{
}

Piece::Piece(const Piece_Color color, const Piece_Type type) noexcept :
    piece_code((static_cast<int>(type) << 1) | static_cast<int>(color)),
    sliding_moves(get_sliding_moves(type)),
    non_sliding_moves(get_non_sliding_moves(type)),
    special_moves(get_special_moves(color, type))
{
    // piece_code layout: 4 bits
    // 3 most significant bits = Piece_Type (values 0-5)
    // least significant bit = Piece_Color (0 or 1)
    //
    // 101 1
    // ^^^ ^-- Piece_Color::BLACK
    //  +--- Piece_Type::KING
}

Piece::Piece(const char pgn_symbol) : Piece(std::isupper(pgn_symbol) ? Piece_Color::WHITE : Piece_Color::BLACK,
                                            piece_type_from_char(pgn_symbol))
{
}

Piece_Color Piece::color() const noexcept
{
    assert(*this);
    return static_cast<Piece_Color>(piece_code & 1);
}

std::string Piece::pgn_symbol() const noexcept
{
    assert(*this);
    return type() == Piece_Type::PAWN ? std::string{} : std::string(1, String::toupper(fen_symbol()));
}

char Piece::fen_symbol() const noexcept
{
    assert(*this);
    const auto symbol = pgn_symbols[static_cast<int>(type())];
    return (color() == Piece_Color::WHITE ? symbol : String::tolower(symbol));
}

bool Piece::can_move(const Move& move) const noexcept
{
    assert(*this);
    for(const auto& moves : move_lists(move.start()))
    {
        if(std::find(moves.begin(), moves.end(), move) != moves.end())
        {
            return true;
        }
    }

    return false;
}

Piece::list_of_move_lists Piece::move_lists(const Square square) const noexcept
{
    assert(*this);
    list_of_move_lists moves;
    for(const auto& step : sliding_moves)
    {
        moves.emplace_back();
        for(const auto target : Squares_in_a_Line(square, step))
        {
            moves.back().emplace_back(square, target);
        }
    }

    for(const auto& move : non_sliding_moves)
    {
        const auto target = square + move;
        if(target.inside_board())
        {
            moves.push_back({ {square, target} });
        }
    }

    for(const auto& special_move_list : special_moves[square.index()])
    {
        moves.emplace_back();
        for(const auto& special_move : special_move_list)
        {
            moves.back().push_back(special_move);
        }
    }

    return moves;
}

Piece_Type Piece::type() const noexcept
{
    assert(*this);
    return static_cast<Piece_Type>(piece_code >> 1);
}

Piece::operator bool() const noexcept
{
    return piece_code != invalid_code;
}

Piece::piece_code_t Piece::index() const noexcept
{
    return piece_code;
}

Piece::list_of_move_lists Piece::attacking_move_lists(const Square square) const noexcept
{
    assert(*this);
    const auto all_moves = move_lists(square);
    list_of_move_lists result;
    for(const auto& move_list : all_moves)
    {
        result.emplace_back();
        for(const auto& move : move_list)
        {
            if(move.can_capture())
            {
                if(move.promotion())
                {
                    if(move.promotion().type() == Piece_Type::QUEEN)
                    {
                        result.back().push_back(move);
                    }
                }
                else
                {
                    result.back().push_back(move);
                }
            }
        }
    }

    return result;
}

bool operator==(const Piece a, const Piece b) noexcept
{
    return a.index() == b.index();
}

bool operator!=(const Piece a, const Piece b) noexcept
{
    return ! (a == b);
}
