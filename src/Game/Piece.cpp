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
            throw std::invalid_argument(pgn_symbol + std::string{" is not a valid piece PGN symbol."});
        }
        return static_cast<Piece_Type>(i);
    }
}

Piece::Piece() noexcept : piece_code(invalid_code)
{
}

Piece::Piece(const Piece_Color color, const Piece_Type type) noexcept :
    piece_code((static_cast<int>(type) << 1) | static_cast<int>(color))
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
    for(const auto& move : sliding_moves)
    {
        moves.emplace_back();
        const auto step = move.movement();
        for(auto target = square + step; target.inside_board(); target += step)
        {
            moves.back().emplace_back(square, target);
        }
    }

    for(const auto& move : non_sliding_moves)
    {
        moves.push_back({move});
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
