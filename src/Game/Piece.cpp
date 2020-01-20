#include "Game/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <memory>

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
    using indexed_move_array = std::array<std::array<std::array<std::vector<std::vector<const Move*>>, 64>, 6>, 2>;
    using indexed_art_array = std::array<std::array<std::vector<std::string>, 6>, 2>;


    void add_pawn_moves(indexed_move_array& out, Color color) noexcept;
    void add_rook_moves(indexed_move_array& out, Color color, Piece_Type type = ROOK) noexcept;
    void add_knight_moves(indexed_move_array& out, Color color) noexcept;
    void add_bishop_moves(indexed_move_array& out, Color color, Piece_Type type = BISHOP) noexcept;
    void add_queen_moves(indexed_move_array& out, Color color) noexcept;
    void add_king_moves(indexed_move_array& out, Color color) noexcept;

    void add_pawn_art(indexed_art_array& out, Color color) noexcept;
    void add_rook_art(indexed_art_array& out, Color color) noexcept;
    void add_knight_art(indexed_art_array& out, Color color) noexcept;
    void add_bishop_art(indexed_art_array& out, Color color) noexcept;
    void add_queen_art(indexed_art_array& out, Color color) noexcept;
    void add_king_art(indexed_art_array& out, Color color) noexcept;

    const auto ascii_art_lines =
        []()
        {
            indexed_art_array result;

            for(auto color : {WHITE, BLACK})
            {
                add_pawn_art(result, color);
                add_rook_art(result, color);
                add_knight_art(result, color);
                add_bishop_art(result, color);
                add_queen_art(result, color);
                add_king_art(result, color);
            }

            return result;
        }();

    const auto maximum_piece_height =
        []()
        {
            size_t max_height = 0;
            for(auto color : {WHITE, BLACK})
            {
                for(auto type : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING})
                {
                    max_height = std::max(max_height, ascii_art_lines[color][type].size());
                }
            }

            return max_height;
        }();

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
                    for(size_t index = 0; index < 64; ++index)
                    {
                        for(const auto& move_list : legal_moves[color][type][index])
                        {
                            result[color][type][index].push_back({});

                            for(auto move : move_list)
                            {
                                // Make list of all capturing moves, excluding all but one type of pawn capture per square.
                                if(move->can_capture()
                                    && ! move->is_en_passant()
                                    && (move->promotion_piece_symbol() == 'Q' || move->promotion_piece_symbol() == '\0'))
                                {
                                    result[color][type][index].back().push_back(move);
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
    void add_legal_move(indexed_move_array& out, Color color, Piece_Type type, bool add_new_list, Parameters ... parameters) noexcept
    {
        auto move = std::make_unique<Move_Type>(parameters...);
        auto index = move->start().index();
        auto& lists = out[color][type][index];
        if(lists.empty() || add_new_list)
        {
            lists.push_back({});
        }

        if( ! lists.back().empty() && ! same_direction(move->movement(), lists.back().back()->movement()))
        {
            lists.push_back({});
        }

        lists.back().push_back(move.release());
    }

    void add_standard_legal_move(indexed_move_array& out, Color color, Piece_Type type, int file_step, int rank_step) noexcept
    {
        for(auto start : Square::all_squares())
        {
            auto end = start + Square_Difference{file_step, rank_step};
            if(end.inside_board())
            {
                add_legal_move<Move>(out, color, type, false, start, end);
            }
        }
    }

    void add_pawn_moves(indexed_move_array& out, Color color) noexcept
    {
        auto base_rank = (color == WHITE ? 2 : 7);
        auto no_normal_move_rank = (color == WHITE ? 7 : 2);
        auto direction = (color == WHITE ? 1 : -1);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_legal_move<Pawn_Move>(out, color, PAWN, true, color, Square{file, rank});
            }
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            add_legal_move<Pawn_Double_Move>(out, color, PAWN, false, color, file);
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
                    add_legal_move<Pawn_Capture>(out, color, PAWN, true, color, dir, Square{file, rank});
                }
            }

            for(char file = first_file; file <= last_file; ++file)
            {
                add_legal_move<En_Passant>(out, color, PAWN, true, color, dir, file);
            }

            for(auto promote : possible_promotions)
            {
                for(auto file = first_file; file <= last_file; ++file)
                {
                    add_legal_move<Pawn_Promotion_by_Capture>(out, color, PAWN, true, promote, color, dir, file);
                }
            }
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = 'a'; file <= 'h'; ++file)
            {
                add_legal_move<Pawn_Promotion>(out, color, PAWN, true, promote, color, file);
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
                    add_standard_legal_move(out, color, type, move_size*d_file, move_size*d_rank);
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
                    add_standard_legal_move(out, color, KNIGHT, d_file*file_direction, d_rank*rank_direction);
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
                    add_standard_legal_move(out, color, type, move_size*d_file, move_size*d_rank);
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
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            for(int d_file = -1; d_file <= 1; ++d_file)
            {
                if(d_rank == 0 && d_file == 0) { continue; }

                add_standard_legal_move(out, color, KING, d_file, d_rank);
            }
        }

        int base_rank = (color == WHITE ? 1 : 8);
        add_legal_move<Castle>(out, color, KING, true, base_rank, Direction::LEFT);
        add_legal_move<Castle>(out, color, KING, true, base_rank, Direction::RIGHT);
    }

    void add_color(indexed_art_array& out, Color color, Piece_Type type) noexcept
    {
        if(color == WHITE)
        {
            return;
        }

        for(auto& line : out[color][type])
        {
            std::replace_if(line.begin(), line.end(), [](auto c){ return c == ' ' || c == '_'; }, '#');
        }
    }

    void add_pawn_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII Art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][PAWN];
        store.insert(store.end(), {"( )",
                                  "/___\\"});
        add_color(out, color, PAWN);
    }

    void add_rook_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII Art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][ROOK];
        store.insert(store.end(), {"|U|",
                                   "| |",
                                  "/___\\"});
        add_color(out, color, ROOK);
    }

    void add_knight_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII Art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][KNIGHT];
        store.insert(store.end(), {"/\")",
                                   "7 (",
                                  "/___\\"});
        add_color(out, color, KNIGHT);
    }

    void add_bishop_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][BISHOP];
        store.insert(store.end(), {"(V)",
                                   ") (",
                                  "/___\\"});
        add_color(out, color, BISHOP);
    }

    void add_queen_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII Art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][QUEEN];
        store.insert(store.end(), {"\\^/",
                                    ") (",
                                   "(___)"});
        add_color(out, color, QUEEN);
    }

    void add_king_art(indexed_art_array& out, Color color) noexcept
    {
        // ASCII Art http://ascii.co.uk/art/chess (VK)
        auto& store = out[color][KING];
        store.insert(store.end(), {"\\+/",
                                    "| |",
                                   "/___\\"});
        add_color(out, color, KING);
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

std::string Piece::ascii_art(size_t row, size_t square_height) const noexcept
{
    assert(*this);
    assert(square_height >= maximum_piece_height);

    const auto& art = ascii_art_lines[color()][type()];
    auto empty_bottom_rows = (square_height - maximum_piece_height)/2;
    auto empty_top_rows = square_height - art.size() - empty_bottom_rows;
    auto line = row - empty_top_rows;
    if(line < art.size())
    {
        return art[line];
    }
    else
    {
        return {};
    }
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
    return legal_moves[color()][type()][square.index()];
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
    return attack_moves[color()][type()][square.index()];
}

bool operator==(Piece a, Piece b) noexcept
{
    return a.index() == b.index();
}

bool operator!=(Piece a, Piece b) noexcept
{
    return !(a == b);
}
