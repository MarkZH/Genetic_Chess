#include "Game/Musketeer_Board.h"

#include <string>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <iterator>

#include "Game/Piece.h"
#include "Moves/Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Direction.h"
#include "Game/Color.h"
#include "Game/Board.h"
#include "Game/Game_Result.h"

#include "Utility/String.h"
#include "Utility/Random.h"

namespace
{
    std::string extract_standard_fen(const std::string& original_fen)
    {
        if( ! String::contains(original_fen, "*"))
        {
            throw std::runtime_error("No gate rows in board field: " + original_fen);
        }

        auto tokens = String::split(original_fen);
        auto board_rows = String::split(tokens.front(), "/");
        if(board_rows.size() != 10)
        {
            throw std::runtime_error("Wrong number of rows in board field: " + original_fen);
        }

        auto standard_board = String::join(std::next(board_rows.begin()), std::prev(board_rows.end()), "/");
        return standard_board + " " + String::join(std::next(tokens.begin()), tokens.end(), " ");
    }
}

Musketeer_Board::Musketeer_Board() noexcept : Board()
{
    pick_and_place_random_gated_pieces();
    set_unmoved_gate_guardians();
    set_initial_fen(fen());
}

Musketeer_Board::Musketeer_Board(const std::string& input_fen) : Board(extract_standard_fen(input_fen))
{
    auto tokens = String::split(input_fen);
    auto board_rows = String::split(tokens.front(), "/");
    for(auto gate_index : {0, 9})
    {
        auto gate = board_rows[gate_index];
        auto gate_color = gate_index == 0 ? Piece_Color::BLACK : Piece_Color::WHITE;
        for(size_t i = 0; i < gate.size(); ++i)
        {
            if(gate[i] != '*')
            {
                gated_pieces[static_cast<int>(gate_color)][i] = Piece{gate[i]};
            }
        }
    }

    if(fen() != String::remove_extra_whitespace(input_fen))
    {
        throw std::runtime_error("Input FEN not preserved.\nOriginal: " + input_fen + "\nResult: " + fen());
    }

    if(ply_count() == 0)
    {
        pick_and_place_random_gated_pieces();
    }

    set_unmoved_gate_guardians();

    set_initial_fen(fen());
}

std::unique_ptr<Board> Musketeer_Board::copy() const noexcept
{
    return std::unique_ptr<Musketeer_Board>(new Musketeer_Board(*this));
}

std::string Musketeer_Board::fen() const noexcept
{
    auto standard_fen = Board::fen();
    auto tokens = String::split(standard_fen);
    auto board_spec = gate_fen(Piece_Color::BLACK) + "/" + tokens.front() + "/" + gate_fen(Piece_Color::WHITE);
    return board_spec + " " + String::join(std::next(tokens.begin()), tokens.end(), " ");
}

std::string Musketeer_Board::extra_move_mark(const Move& move) const noexcept
{
    auto next_board = copy();
    next_board->submit_move(move);
    auto gated_piece = const_cast<const Board*>(next_board.get())->piece_on_square(move.start());
    if(gated_piece)
    {
        return "/" + gated_piece.pgn_symbol();
    }
    else
    {
        return {};
    }
}

void Musketeer_Board::other_move_effects(const Move& move) noexcept
{
    auto base_rank = whose_turn() == Piece_Color::WHITE ? 1 : 8;
    if(move.start().rank() == base_rank)
    {
        auto color_index = static_cast<int>(whose_turn());
        auto file_index = move.start().file() - 'a';
        auto gated_piece = gated_pieces[color_index][file_index];
        if(gated_piece)
        {
            place_piece(gated_piece, move.start());
            gated_pieces[color_index][file_index] = {};
        }
    }
}

void Musketeer_Board::pick_and_place_random_gated_pieces() noexcept
{
    static const auto choices = std::array<Piece_Type, 10>{Piece_Type::LEOPARD,
                                                           Piece_Type::CANNON,
                                                           Piece_Type::UNICORN,
                                                           Piece_Type::DRAGON,
                                                           Piece_Type::CHANCELLOR,
                                                           Piece_Type::ARCHBISHOP,
                                                           Piece_Type::ELEPHANT,
                                                           Piece_Type::HAWK,
                                                           Piece_Type::FORTRESS,
                                                           Piece_Type::SPIDER};
    std::vector<Piece_Type> gated_piece_types;
    for(const auto& list : gated_pieces)
    {
        for(const auto& piece : list)
        {
            if(piece && gated_piece_types.size() < 2)
            {
                if(gated_piece_types.empty() || gated_piece_types.front() != piece.type())
                {
                    gated_piece_types.push_back(piece.type());
                }
            }
        }
    }

    while(gated_piece_types.size() < 2)
    {
        auto type = Random::random_element(choices);
        if(gated_piece_types.empty() || gated_piece_types.front() != type)
        {
            gated_piece_types.push_back(type);
        }
    }

    for(auto color : {Piece_Color::WHITE, Piece_Color::BLACK})
    {
        auto& gate_list = gated_pieces[static_cast<int>(color)];
        for(auto type : gated_piece_types)
        {
            auto gated_piece = Piece{color, type};
            if(std::find(gate_list.begin(), gate_list.end(), gated_piece) != gate_list.end())
            {
                continue;
            }

            auto other_index = std::distance(gate_list.begin(),
                                             std::find_if(gate_list.begin(), gate_list.end(),
                                                          [](const auto& piece)
                                                          {
                                                              return bool(piece);
                                                          }));

            while(true)
            {
                auto file_index = Random::random_integer(0, 7);
                if(file_index == other_index)
                {
                    continue;
                }

                // Cannot place gated pieces behind rook and king
                if(file_index == 4 && (other_index == 0 || other_index == 7))
                {
                    continue;
                }

                // Cannot place gated pieces behind rook and king
                if(other_index == 4 && (file_index == 0 || file_index == 7))
                {
                    continue;
                }

                gate_list[file_index] = gated_piece;
                break;
            }
        }
    }

    for(const auto& piece_type : gated_piece_types)
    {
        for(auto color : {Piece_Color::WHITE, Piece_Color::BLACK})
        {
            auto& promotion_list = gated_pawn_promotions[static_cast<int>(color)];
            for(auto file = 'a'; file <= 'h'; ++file)
            {
                promotion_list.emplace_back(std::make_shared<const Pawn_Promotion>(piece_type, color, file));
                if(file > 'a')
                {
                    promotion_list.emplace_back(std::make_shared<const Pawn_Promotion>(piece_type, color, file, Direction::LEFT));
                }

                if(file < 'h')
                {
                    promotion_list.emplace_back(std::make_shared<const Pawn_Promotion>(piece_type, color, file, Direction::RIGHT));
                }
            }
        }
    }
}

void Musketeer_Board::set_unmoved_gate_guardians() noexcept
{
    for(auto base_rank : {1, 8})
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            auto gate_index = file - 'a';
            const auto& player_gated_pieces = gated_pieces[base_rank == 1 ? 0 : 1];
            if(player_gated_pieces[gate_index])
            {
                set_unmoved({file, base_rank});
            }
        }
    }
}

std::string Musketeer_Board::gate_fen(Piece_Color color) const noexcept
{
    std::string result;
    for(auto piece : gated_pieces[static_cast<int>(color)])
    {
        result.push_back(piece ? piece.fen_symbol() : '*');
    }
    return result;
}

void Musketeer_Board::ascii_draw_above_board(int indentation, int symbol_width) const noexcept
{
    ascii_draw_gate(Piece_Color::BLACK, indentation, symbol_width);
}

void Musketeer_Board::ascii_draw_gate(Piece_Color color, int indentation, int symbol_width) const noexcept
{
    std::cout << std::setw(indentation) << std::left << "" << std::right;
    for(auto piece : gated_pieces[static_cast<int>(color)])
    {
        if(piece)
        {
            std::cout << std::setw(symbol_width) << piece.fen_symbol();
        }
        else
        {
            std::cout << std::setw(symbol_width) << '#';
        }
    }
    std::cout << std::endl;
}

void Musketeer_Board::ascii_draw_below_board(int indentation, int symbol_width) const noexcept
{
    ascii_draw_gate(Piece_Color::WHITE, indentation, symbol_width);
}

void Musketeer_Board::add_other_moves(std::vector<const Move*>& move_list) noexcept
{
    auto pawn = Piece{whose_turn(), Piece_Type::PAWN};
    for(const auto& move : gated_pawn_promotions[static_cast<int>(whose_turn())])
    {
        if(const_cast<const Musketeer_Board&>(*this).piece_on_square(move->start()) == pawn)
        {
            move_list.push_back(move.get());
        }
    }
}
