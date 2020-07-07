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

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/En_Passant.h"
#include "Moves/Castle.h"

#include "Utility/Fixed_Capacity_Vector.h"

const Piece::piece_code_t Piece::invalid_code = Piece{Piece_Color::BLACK, Piece_Type::KING}.index() + 1;

namespace
{
    const std::string pgn_symbols = "PRNBQK";

    using indexed_move_array = std::array<std::array<Fixed_Capacity_Vector<Fixed_Capacity_Vector<const Move*, 7>, 12>, 64>, 12>;

    void add_pawn_moves(indexed_move_array& out, Piece_Color color) noexcept;
    void add_rook_moves(indexed_move_array& out, Piece_Color color, Piece_Type type = Piece_Type::ROOK) noexcept;
    void add_knight_moves(indexed_move_array& out, Piece_Color color) noexcept;
    void add_bishop_moves(indexed_move_array& out, Piece_Color color, Piece_Type type = Piece_Type::BISHOP) noexcept;
    void add_queen_moves(indexed_move_array& out, Piece_Color color) noexcept;
    void add_king_moves(indexed_move_array& out, Piece_Color color) noexcept;

    const auto legal_moves =
        []()
        {
            indexed_move_array result;
            for(auto color : {Piece_Color::WHITE, Piece_Color::BLACK})
            {
                add_pawn_moves(result, color);
                add_rook_moves(result, color);
                add_knight_moves(result, color);
                add_bishop_moves(result, color);
                add_queen_moves(result, color);
                add_king_moves(result, color);
            }

            return result;
        }();


    const auto attack_moves =
        []()
        {
            indexed_move_array result;
            for(auto color : {Piece_Color::WHITE, Piece_Color::BLACK})
            {
                for(auto type_index = 0; type_index <= static_cast<int>(Piece_Type::KING); ++type_index)
                {
                    auto piece = Piece{color, static_cast<Piece_Type>(type_index)};
                    for(size_t index = 0; index < 64; ++index)
                    {
                        for(const auto& move_list : legal_moves[piece.index()][index])
                        {
                            result[piece.index()][index].push_back({});

                            for(auto move : move_list)
                            {
                                // Make list of all capturing moves, excluding all but one type of pawn capture per square.
                                if(move->can_capture()
                                    && ! move->is_en_passant()
                                    && (move->promotion_piece_symbol() == 'Q' || move->promotion_piece_symbol() == '\0'))
                                {
                                    result[piece.index()][index].back().push_back(move);
                                }
                            }
                        }
                    }
                }
            }

            return result;
        }();


    // Add a move to the list that is only legal when starting from a certain square
    // (e.g., castling, pawn double move, promotion, etc.)
    template<typename Move_Type, typename ...Parameters>
    void add_legal_move(indexed_move_array& out, Piece piece, bool blockable, Parameters ... parameters) noexcept
    {
        auto move = new Move_Type(parameters...);
        auto& lists = out[piece.index()][move->start().index()];
        if(lists.empty() || ! blockable)
        {
            lists.push_back({});
        }

        if( ! lists.back().empty() && ! same_direction(move->movement(), lists.back().back()->movement()))
        {
            lists.push_back({});
        }

        lists.back().push_back(move);
    }

    void add_standard_legal_move(indexed_move_array& out, Piece piece, int file_step, int rank_step, bool blockable) noexcept
    {
        for(auto start : Square::all_squares())
        {
            auto end = start + Square_Difference{file_step, rank_step};
            if(end.inside_board())
            {
                add_legal_move<Move>(out, piece, blockable, start, end);
            }
        }
    }

    void add_pawn_moves(indexed_move_array& out, Piece_Color color) noexcept
    {
        auto pawn = Piece{color, Piece_Type::PAWN};
        auto base_rank = (color == Piece_Color::WHITE ? 2 : 7);
        auto no_normal_move_rank = (color == Piece_Color::WHITE ? 7 : 2);
        auto rank_change = (color == Piece_Color::WHITE ? 1 : -1);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += rank_change)
            {
                add_legal_move<Pawn_Move>(out, pawn, true, color, Square{file, rank});
            }
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            add_legal_move<Pawn_Double_Move>(out, pawn, true, color, file);
        }

        std::vector<Piece_Type> possible_promotions;
        for(auto type_index = 0; type_index <= static_cast<int>(Piece_Type::KING); ++type_index)
        {
            auto type = static_cast<Piece_Type>(type_index);
            if(type == Piece_Type::PAWN || type == Piece_Type::KING)
            {
                continue;
            }
            possible_promotions.push_back(type);
        }

        for(auto dir : {Direction::RIGHT, Direction::LEFT})
        {
            auto first_file = (dir == Direction::RIGHT ? 'a' : 'b');
            auto last_file = (dir == Direction::RIGHT ? 'g' : 'h');
            for(char file = first_file; file <= last_file; ++file)
            {
                for(int rank = base_rank; rank != no_normal_move_rank; rank += rank_change)
                {
                    add_legal_move<Pawn_Move>(out, pawn, true, color, Square{file, rank}, dir);
                }
            }

            for(char file = first_file; file <= last_file; ++file)
            {
                add_legal_move<En_Passant>(out, pawn, true, color, dir, file);
            }

            for(auto promote : possible_promotions)
            {
                for(auto file = first_file; file <= last_file; ++file)
                {
                    add_legal_move<Pawn_Promotion>(out, pawn, false, promote, color, file, dir);
                }
            }
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = 'a'; file <= 'h'; ++file)
            {
                add_legal_move<Pawn_Promotion>(out, pawn, false, promote, color, file);
            }
        }
    }

    void add_rook_moves(indexed_move_array& out, Piece_Color color, Piece_Type type) noexcept
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            for(int d_rank = -1; d_rank <= 1; ++d_rank)
            {
                if(d_file == 0 && d_rank == 0) { continue; }
                if(d_file != 0 && d_rank != 0) { continue; }

                for(int move_size = 1; move_size <= 7; ++move_size)
                {
                    add_standard_legal_move(out, {color, type}, move_size*d_file, move_size*d_rank, true);
                }
            }
        }
    }

    void add_knight_moves(indexed_move_array& out, Piece_Color color) noexcept
    {
        for(auto d_file : {1, 2})
        {
            auto d_rank = 3 - d_file;
            for(auto file_direction : {-1, 1})
            {
                for(auto rank_direction : {-1, 1})
                {
                    add_standard_legal_move(out, {color, Piece_Type::KNIGHT}, d_file*file_direction, d_rank*rank_direction, false);
                }
            }
        }
    }

    void add_bishop_moves(indexed_move_array& out, Piece_Color color, Piece_Type type) noexcept
    {
        for(int d_rank : {-1, 1})
        {
            for(int d_file : {-1, 1})
            {
                for(int move_size = 1; move_size <= 7; ++move_size)
                {
                    add_standard_legal_move(out, {color, type}, move_size*d_file, move_size*d_rank, true);
                }
            }
        }
    }

    void add_queen_moves(indexed_move_array& out, Piece_Color color) noexcept
    {
        add_bishop_moves(out, color, Piece_Type::QUEEN);
        add_rook_moves(out, color, Piece_Type::QUEEN);
    }

    void add_king_moves(indexed_move_array& out, Piece_Color color) noexcept
    {
        auto king = Piece{color, Piece_Type::KING};
        int base_rank = (color == Piece_Color::WHITE ? 1 : 8);
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            for(int d_file = -1; d_file <= 1; ++d_file)
            {
                if(d_rank == 0 && d_file == 0) { continue; }

                add_standard_legal_move(out, king, d_file, d_rank, true);
                if(d_rank == 0)
                {
                    if(d_file > 0)
                    {
                        add_legal_move<Castle>(out, king, true, base_rank, Direction::RIGHT);
                    }
                    else
                    {
                        add_legal_move<Castle>(out, king, true, base_rank, Direction::LEFT);
                    }
                }
            }
        }
    }

    Piece_Type piece_type_from_char(char pgn_symbol)
    {
        auto i = pgn_symbols.find(std::toupper(pgn_symbol));
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

Piece::Piece(Piece_Color color, Piece_Type type) noexcept :
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

Piece::Piece(char pgn_symbol) : Piece(std::isupper(pgn_symbol) ? Piece_Color::WHITE : Piece_Color::BLACK,
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
    return type() == Piece_Type::PAWN ? std::string{} : std::string(1, std::toupper(fen_symbol()));
}

char Piece::fen_symbol() const noexcept
{
    assert(*this);
    auto symbol = pgn_symbols[static_cast<int>(type())];
    return (color() == Piece_Color::WHITE ? symbol : std::tolower(symbol));
}

bool Piece::can_move(const Move* move) const noexcept
{
    assert(*this);
    for(const auto& moves : move_lists(move->start()))
    {
        if(std::find(moves.begin(), moves.end(), move) != moves.end())
        {
            return true;
        }
    }

    return false;
}

const Piece::list_of_move_lists& Piece::move_lists(Square square) const noexcept
{
    assert(*this);
    return legal_moves[index()][square.index()];
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

const Piece::list_of_move_lists& Piece::attacking_move_lists(Square square) const noexcept
{
    assert(*this);
    return attack_moves[index()][square.index()];
}

bool operator==(Piece a, Piece b) noexcept
{
    return a.index() == b.index();
}

bool operator!=(Piece a, Piece b) noexcept
{
    return !(a == b);
}
