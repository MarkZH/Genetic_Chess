#include "Game/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>

#include "Game/Square.h"
#include "Game/Color.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Pawn_Promotion_by_Capture.h"
#include "Moves/En_Passant.h"
#include "Moves/Castle.h"

namespace
{
    using indexed_move_array = std::array<std::array<std::vector<std::vector<const Move*>>, 64>, 12>;

    void add_pawn_moves(indexed_move_array& out, Color color) noexcept;
    void add_rook_moves(indexed_move_array& out, Color color, Piece_Type type = ROOK) noexcept;
    void add_knight_moves(indexed_move_array& out, Color color) noexcept;
    void add_bishop_moves(indexed_move_array& out, Color color, Piece_Type type = BISHOP) noexcept;
    void add_queen_moves(indexed_move_array& out, Color color) noexcept;
    void add_king_moves(indexed_move_array& out, Color color) noexcept;

    const auto legal_moves =
        []()
        {
            indexed_move_array result;
            for(auto color : {WHITE, BLACK})
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
            for(auto color : {WHITE, BLACK})
            {
                for(auto type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
                {
                    auto piece = Piece{color, type};
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
    void add_legal_move(indexed_move_array& out, Piece piece, bool add_new_list, Parameters ... parameters) noexcept
    {
        auto move = new Move_Type(parameters...);
        auto& lists = out[piece.index()][move->start().index()];
        if(lists.empty() || add_new_list)
        {
            lists.push_back({});
        }

        if( ! lists.back().empty() && ! same_direction(move->movement(), lists.back().back()->movement()))
        {
            lists.push_back({});
        }

        lists.back().push_back(move);
    }

    void add_standard_legal_move(indexed_move_array& out, Piece piece, int file_step, int rank_step) noexcept
    {
        for(auto start : Square::all_squares())
        {
            auto end = start + Square_Difference{file_step, rank_step};
            if(end.inside_board())
            {
                add_legal_move<Move>(out, piece, false, start, end);
            }
        }
    }

    void add_pawn_moves(indexed_move_array& out, Color color) noexcept
    {
        auto pawn = Piece{color, PAWN};
        auto base_rank = (color == WHITE ? 2 : 7);
        auto no_normal_move_rank = (color == WHITE ? 7 : 2);
        auto direction = (color == WHITE ? 1 : -1);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_legal_move<Pawn_Move>(out, pawn, true, color, Square{file, rank});
            }
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            add_legal_move<Pawn_Double_Move>(out, pawn, false, color, file);
        }

        auto possible_promotions = {QUEEN, KNIGHT, ROOK, BISHOP};

        for(auto dir : {Direction::RIGHT, Direction::LEFT})
        {
            auto first_file = (dir == Direction::RIGHT ? 'a' : 'b');
            auto last_file = (dir == Direction::RIGHT ? 'g' : 'h');
            for(char file = first_file; file <= last_file; ++file)
            {
                for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
                {
                    add_legal_move<Pawn_Capture>(out, pawn, true, color, dir, Square{file, rank});
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
                    add_legal_move<Pawn_Promotion_by_Capture>(out, pawn, true, promote, color, dir, file);
                }
            }
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = 'a'; file <= 'h'; ++file)
            {
                add_legal_move<Pawn_Promotion>(out, pawn, true, promote, color, file);
            }
        }
    }

    void add_rook_moves(indexed_move_array& out, Color color, Piece_Type type) noexcept
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            for(int d_rank = -1; d_rank <= 1; ++d_rank)
            {
                if(d_file == 0 && d_rank == 0) { continue; }
                if(d_file != 0 && d_rank != 0) { continue; }

                for(int move_size = 1; move_size <= 7; ++move_size)
                {
                    add_standard_legal_move(out, {color, type}, move_size*d_file, move_size*d_rank);
                }
            }
        }
    }

    void add_knight_moves(indexed_move_array& out, Color color) noexcept
    {
        for(auto d_file : {1, 2})
        {
            auto d_rank = 3 - d_file;
            for(auto file_direction : {-1, 1})
            {
                for(auto rank_direction : {-1, 1})
                {
                    add_standard_legal_move(out, {color, KNIGHT}, d_file * file_direction, d_rank * rank_direction);
                }
            }
        }
    }

    void add_bishop_moves(indexed_move_array& out, Color color, Piece_Type type) noexcept
    {
        for(int d_rank : {-1, 1})
        {
            for(int d_file : {-1, 1})
            {
                for(int move_size = 1; move_size <= 7; ++move_size)
                {
                    add_standard_legal_move(out, {color, type}, move_size * d_file, move_size * d_rank);
                }
            }
        }
    }

    void add_queen_moves(indexed_move_array& out, Color color) noexcept
    {
        add_bishop_moves(out, color, QUEEN);
        add_rook_moves(out, color, QUEEN);
    }

    void add_king_moves(indexed_move_array& out, Color color) noexcept
    {
        auto king = Piece{color, KING};
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            for(int d_file = -1; d_file <= 1; ++d_file)
            {
                if(d_rank == 0 && d_file == 0) { continue; }

                add_standard_legal_move(out, king, d_file, d_rank);
            }
        }

        int base_rank = (color == WHITE ? 1 : 8);
        add_legal_move<Castle>(out, king, true, base_rank, Direction::LEFT);
        add_legal_move<Castle>(out, king, true, base_rank, Direction::RIGHT);
    }
}

const Piece::piece_code_t Piece::invalid_code = Piece{BLACK, KING}.index() + 1;

Piece::Piece() noexcept : piece_code(invalid_code)
{
}

Piece::Piece(Color color, Piece_Type type) noexcept :
    piece_code((type << 1) | color)
{
    // piece_code layout: 4 bits
    // 3 most significant bits = Piece_Type (values 0-5)
    // least significant bit = Color (0 or 1)
    //
    // 101 1
    // ^^^ ^-- BLACK
    //  +--- KING

    assert(color < 2 && type < 6);
}

Color Piece::color() const noexcept
{
    assert(*this);
    return static_cast<Color>(piece_code & 1);
}

std::string Piece::pgn_symbol() const noexcept
{
    assert(*this);
    return type() == PAWN ? std::string{} : std::string(1, std::toupper(fen_symbol()));
}

char Piece::fen_symbol() const noexcept
{
    assert(*this);
    static const auto symbols = "PRNBQK";
    auto symbol = symbols[type()];
    return (color() == WHITE ? symbol : std::tolower(symbol));
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

const std::vector<std::vector<const Move*>>& Piece::move_lists(Square square) const noexcept
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

const std::vector<std::vector<const Move*>>& Piece::attacking_move_lists(Square square) const noexcept
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
