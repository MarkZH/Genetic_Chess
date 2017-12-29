#include <iostream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cassert>
#include <stdexcept>
#include <map>
#include <array>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Square.h"

#include "Pieces/Piece.h"
#include "Pieces/Pawn.h"
#include "Pieces/Rook.h"
#include "Pieces/Knight.h"
#include "Pieces/Bishop.h"
#include "Pieces/King.h"
#include "Pieces/Queen.h"

#include "Moves/Move.h"
#include "Moves/Move.h"

#include "Exceptions/Illegal_Move_Exception.h"
#include "Exceptions/Promotion_Exception.h"

#include "Utility.h"

// NOTE: The pawn must be created last since it takes pointers to the other pieces
const Rook   Board::white_rook(WHITE);
const Knight Board::white_knight(WHITE);
const Bishop Board::white_bishop(WHITE);
const Queen  Board::white_queen(WHITE);
const King   Board::white_king(WHITE);
const Pawn   Board::white_pawn(WHITE);

const Rook   Board::black_rook(BLACK);
const Knight Board::black_knight(BLACK);
const Bishop Board::black_bishop(BLACK);
const Queen  Board::black_queen(BLACK);
const King   Board::black_king(BLACK);
const Pawn   Board::black_pawn(BLACK);

const Pawn* Board::get_pawn(Color color)
{
    return color == WHITE ? &white_pawn : &black_pawn;
}

const Rook* Board::get_rook(Color color)
{
    return color == WHITE ? &white_rook : &black_rook;
}

const Knight* Board::get_knight(Color color)
{
    return color == WHITE ? &white_knight : &black_knight;
}

const Bishop* Board::get_bishop(Color color)
{
    return color == WHITE ? &white_bishop : &black_bishop;
}

const Queen* Board::get_queen(Color color)
{
    return color == WHITE ? &white_queen : &black_queen;
}

const King* Board::get_king(Color color)
{
    return color == WHITE ? &white_king : &black_king;
}


Board::Board() :
    board{},
    turn_color(WHITE),
    unmoved_positions{},
    en_passant_target({'\0', 0}),
    king_location{{ {'\0', 0}, {'\0', 0} }},
    move_count_start_offset(0),
    first_player_to_move(WHITE),
    capturing_move_available(false),
    thinking_indicator(NO_THINKING)
{
    for(auto color : {WHITE, BLACK})
    {
        int base_rank = (color == WHITE ? 1 : 8);
        place_piece(get_rook(color),   'a', base_rank);
        place_piece(get_knight(color), 'b', base_rank);
        place_piece(get_bishop(color), 'c', base_rank);
        place_piece(get_queen(color),  'd', base_rank);
        place_piece(get_king(color),   'e', base_rank);
        place_piece(get_bishop(color), 'f', base_rank);
        place_piece(get_knight(color), 'g', base_rank);
        place_piece(get_rook(color),   'h', base_rank);

        // Unmoved pieces for castling
        unmoved_positions[Board::board_index('a', base_rank)] = true; // Rook
        unmoved_positions[Board::board_index('e', base_rank)] = true; // King
        unmoved_positions[Board::board_index('h', base_rank)] = true; // Rook

        auto pawn_rank = (base_rank == 1 ? 2 : 7);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            place_piece(get_pawn(color), file, pawn_rank);
        }
    }

    assert(king_location[WHITE]);
    assert(king_location[BLACK]);

    ++repeat_count[board_status()]; // Count initial position
    recreate_move_caches();
}

Board::Board(const std::string& fen) :
    board{},
    turn_color(WHITE),
    unmoved_positions{},
    en_passant_target({'\0', 0}),
    starting_fen(fen),
    king_location{{ {'\0', 0}, {'\0', 0} }},
    capturing_move_available(false),
    thinking_indicator(NO_THINKING)
{
    auto fen_parse = String::split(fen);
    if(fen_parse.size() != 6)
    {
        throw std::runtime_error("Wrong number of fields (should be 6): " + fen);
    }

    auto board_parse = String::split(fen_parse.at(0), "/");
    for(int rank = 8; rank >= 1; --rank)
    {
        char file = 'a';
        for(const auto& symbol : board_parse.at(8-rank))
        {
            if(isdigit(symbol))
            {
                file += symbol - '0';
            }
            else
            {
                Color color = (isupper(symbol) ? WHITE : BLACK);
                switch(toupper(symbol))
                {
                    case 'P':
                        place_piece(get_pawn(color), file, rank);
                        break;
                    case 'R':
                        place_piece(get_rook(color), file, rank);
                        break;
                    case 'N':
                        place_piece(get_knight(color), file, rank);
                        break;
                    case 'B':
                        place_piece(get_bishop(color), file, rank);
                        break;
                    case 'Q':
                        place_piece(get_queen(color), file, rank);
                        break;
                    case 'K':
                        if(king_location[color])
                        {
                            throw std::runtime_error("More than one " + color_text(color) + " king in FEN: " + fen);
                        }
                        place_piece(get_king(color), file, rank);
                        break;
                    default:
                        throw std::runtime_error(std::string("Invalid  symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }
    }

    if( ! king_location[WHITE])
    {
        throw std::runtime_error("White king not in FEN string: " + fen);
    }
    if( ! king_location[BLACK])
    {
        throw std::runtime_error("Black king not in FEN string: " + fen);
    }

    set_turn(fen_parse[1] == "w" ? WHITE : BLACK);
    first_player_to_move = turn_color;

    auto non_turn_color = opposite(whose_turn());
    auto non_turn_king_square = king_location[non_turn_color];
    if( ! safe_for_king(non_turn_king_square.file, non_turn_king_square.rank, non_turn_color))
    {
        throw std::runtime_error(color_text(opposite(whose_turn())) +
                                 " is in check but it is " +
                                 color_text(whose_turn()) + "'s turn.");
    }

    auto castling_parse = fen_parse.at(2);
    if(String::contains(castling_parse, 'K'))
    {
        unmoved_positions[Board::board_index('h', 1)] = true;
        unmoved_positions[Board::board_index('e', 1)] = true;
    }
    if(String::contains(castling_parse, 'Q'))
    {
        unmoved_positions[Board::board_index('a', 1)] = true;
        unmoved_positions[Board::board_index('e', 1)] = true;
    }
    if(String::contains(castling_parse, 'k'))
    {
        unmoved_positions[Board::board_index('h', 8)] = true;
        unmoved_positions[Board::board_index('e', 8)] = true;
    }
    if(String::contains(castling_parse, 'q'))
    {
        unmoved_positions[Board::board_index('a', 8)] = true;
        unmoved_positions[Board::board_index('e', 8)] = true;
    }

    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        make_en_passant_targetable(en_passant_parse[0], en_passant_parse[1] - '0');
    }

    // Fill repeat counter to indicate moves since last
    // pawn move or capture.
    auto fifty_move_count = std::stoul(fen_parse.at(4));
    while(repeat_count.size() < fifty_move_count)
    {
        repeat_count[std::to_string(repeat_count.size())] = 1;
    }
    ++repeat_count[board_status()]; // Count initial position

    move_count_start_offset = std::stoul(fen_parse.at(5)) - 1;
    recreate_move_caches();
}

size_t Board::board_index(char file, int rank)
{
    // Square A1 = Board::board[0]
    // Square H1 = Board::board[7]
    // Square A8 = Board::board[56]
    // Square H8 = Board::board[63]
    assert(inside_board(file, rank));
    return (file - 'a') + 8*(rank - 1);
}

const Piece*& Board::piece_on_square(char file, int rank)
{
    return board[board_index(file, rank)];
}

const Piece* Board::piece_on_square(char file, int rank) const
{
    return board[board_index(file, rank)];
}

bool Board::inside_board(char file, int rank)
{
    return inside_board(file) && inside_board(rank);
}

bool Board::inside_board(char file)
{
    return file >= 'a' && file <= 'h';
}

bool Board::inside_board(int rank)
{
    return rank >= 1 && rank <= 8;
}

Color Board::square_color(char file, int rank)
{
    return (file - 'a') % 2 == (rank - 1) % 2 ? BLACK : WHITE;
}

bool Board::is_legal(char file_start, int rank_start,
                     char file_end,   int rank_end) const
{
    for(const auto& move : legal_moves())
    {
        if(move->start_file() == file_start &&
           move->start_rank() == rank_start &&
           move->end_file() == file_end &&
           move->end_rank() == rank_end)
        {
            return true;
        }
    }

    return false;
}

bool Board::is_in_legal_moves_list(const Move& move) const
{
    return std::find(legal_moves().begin(), legal_moves().end(), &move) != legal_moves().end();
}

std::string Board::fen_status() const
{
    std::string s = board_status() + " ";

    int moves_since_pawn_or_capture = -1; // -1 to not count current state of board
    for(const auto& board_count : repeat_count)
    {
        moves_since_pawn_or_capture += board_count.second;
    }
    s.append(std::to_string(moves_since_pawn_or_capture));
    s.push_back(' ');
    if(starting_fen.empty())
    {
        s.append(std::to_string(1 + game_record.size()/2));
    }
    else
    {
        auto move_number = std::stoul(String::split(starting_fen).back());
        move_number += game_record.size()/2;
        s.append(std::to_string(move_number));
    }

    return s;
}


const Move& Board::get_move(char file_start, int rank_start, char file_end, int rank_end, char promote) const
{
    std::vector<const Move*> move_list;
    for(const auto& move : legal_moves())
    {
        if(move->start_file() == file_start &&
           move->start_rank() == rank_start &&
           move->end_file() == file_end &&
           move->end_rank() == rank_end)
        {
            if(promote)
            {
                if(move->promotion_piece_symbol() == promote)
                {
                    return *move;
                }
            }
            else
            {
                move_list.push_back(move);
            }
        }
    }

    if(move_list.empty())
    {
        throw Illegal_Move_Exception("No legal move found for " +
                                     std::string(1, file_start) +
                                     std::to_string(rank_start) +
                                     "-" +
                                     std::string(1, file_end) +
                                     std::to_string(rank_end) +
                                     " (" + std::string(1, promote ? promote : ' ') + ")");
    }

    if(move_list.size() == 1)
    {
        return *move_list.front();
    }
    else
    {
        throw Promotion_Exception();
    }
}

Game_Result Board::submit_move(const Move& move)
{
    assert(is_in_legal_moves_list(move));
    game_record.push_back(&move);

    make_move(move.start_file(), move.start_rank(),
              move.end_file(),   move.end_rank());
    move.side_effects(*this);

    switch_turn();

    if(no_legal_moves())
    {
        if(king_is_in_check())
        {
            auto winner = opposite(whose_turn());
            return Game_Result(winner, color_text(winner) + " mates", false);
        }
        else
        {
            return Game_Result(NONE, "Stalemate", false);
        }
    }

    // An insufficient material draw can only happen after a capture
    // or a pawn promotion to a minor piece, both of which clear the
    // repeat_count map.
    if(repeat_count.empty() && ! enough_material_to_checkmate())
    {
        return Game_Result(NONE, "Insufficient material", false);
    }

    if(++repeat_count[board_status()] >= 3)
    {
        return Game_Result(NONE, "Threefold repetition", false);
    }

    int fifty_move_count = 0;
    for(const auto& board_count : repeat_count)
    {
        fifty_move_count += board_count.second;
    }
    if(fifty_move_count >= 101) // "Move" means both players move, 101 including current position
    {
        return Game_Result(NONE, "50-move limit", false);
    }

    return {};
}

const Move& Board::get_move(const std::string& move, char promote) const
{
    const std::string pieces = "RNBQK";
    const std::string valid_files = "abcdefgh";
    const std::string valid_rows = "12345678";
    const std::string castling = "O";
    const std::string valid_characters = pieces + valid_files + valid_rows + castling;

    std::string validated;
    char promoted_piece = 0;
    if(String::contains(move, '='))
    {
        for(size_t i = move.find('=') + 1; i < move.size(); ++i)
        {
            if(String::contains(pieces, move[i]) && move[i] != 'K')
            {
                promoted_piece = move[i];
                break;
            }
        }
    }
    else
    {
        promoted_piece = promote;
    }

    for(char c : move)
    {
        if(c == '=')
        {
            break;
        }

        if(String::contains(valid_characters, c))
        {
            validated.push_back(c);
        }
    }

    if(validated.size() < 2)
    {
        throw Illegal_Move_Exception(move + " does not specify a valid move.");
    }

    // Castling
    if(validated == "OO")
    {
        return get_move('e', whose_turn() == WHITE ? 1 : 8,
                        'g', whose_turn() == WHITE ? 1 : 8);
    }
    if(validated == "OOO")
    {
        return get_move('e', whose_turn() == WHITE ? 1 : 8,
                        'c', whose_turn() == WHITE ? 1 : 8);
    }

    // Normal PGN move
    std::string piece_symbol = (String::contains(pieces, validated[0]) ? validated.substr(0, 1) : "");

    char starting_file = 0;
    int  starting_rank = 0;

    if(validated.size() == 5 && ! piece_symbol.empty()) // Bb2c3
    {
        starting_file = validated[1];
        starting_rank = validated[2] - '0';
    }
    else if(validated.size() == 4 && ! piece_symbol.empty()) // Raa5 or R5c5
    {
        if(std::isdigit(validated[1]))
        {
            starting_rank = validated[1] - '0';
        }
        else
        {
            starting_file = validated[1];
        }
    }
    else if(validated.size() <= 3 && piece_symbol.empty()) // Pawn move/capture de5 (dxe5)
    {
        starting_file = validated.front();
    }
    else if(piece_symbol.empty())
    {
        // No PGN-style move works, try coordinate move (e.g., e7e8q)
        if(move.size() < 4 || move.size() > 5)
        {
            throw Illegal_Move_Exception("Illegal text move (wrong length): " + move);
        }

        char start_file = move[0];
        int  start_rank = move[1] - '0';
        char end_file   = move[2];
        int  end_rank   = move[3] - '0';
        if(move.size() == 5)
        {
            promoted_piece = std::toupper(move[4]);
        }

        return get_move(start_file, start_rank, end_file, end_rank, promoted_piece);
    }

    // else normal PGN-style piece movement
    char ending_file = validated[validated.size() - 2];
    int  ending_rank = validated[validated.size() - 1] - '0';
    if( ! inside_board(ending_file, ending_rank))
    {
        throw Illegal_Move_Exception("Illegal text move (out of board): " + move);
    }

    char file_search_start = (starting_file == 0 ? 'a' : starting_file);
    char file_search_end   = (starting_file == 0 ? 'h' : starting_file);

    int rank_search_start = (starting_rank == 0 ? 1 : starting_rank);
    int rank_search_end   = (starting_rank == 0 ? 8 : starting_rank);

    if(starting_file == 0 || starting_rank == 0)
    {
        for(char file = file_search_start; file <= file_search_end; ++file)
        {
            for(int rank = rank_search_start; rank <= rank_search_end; ++rank)
            {
                auto piece = piece_on_square(file, rank);
                if(piece &&
                   piece->pgn_symbol() == piece_symbol &&
                   is_legal(file, rank, ending_file, ending_rank))
                {
                    if(starting_file == 0 || starting_rank == 0)
                    {
                        starting_file = file;
                        starting_rank = rank;
                    }
                    else
                    {
                        // If two moves satisfy text, argument is ambiguous.
                        throw Illegal_Move_Exception("Ambiguous move: " + move);
                    }
                }
            }
        }
    }

    if(starting_file != 0 && starting_rank != 0)
    {
        return get_move(starting_file, starting_rank,
                        ending_file,   ending_rank,
                        promoted_piece);
    }

    throw Illegal_Move_Exception("Malformed move: " + move);
}

void Board::make_move(char file_start, int rank_start, char file_end, int rank_end)
{
    if(piece_on_square(file_end, rank_end)) // capture
    {
        repeat_count.clear();
    }

    place_piece(piece_on_square(file_start, rank_start), file_end, rank_end);
    remove_piece(file_start, rank_start);

    unmoved_positions[Board::board_index(file_start, rank_start)] = false;
    unmoved_positions[Board::board_index(file_end, rank_end)] = false;

    clear_en_passant_target();
}

Color Board::whose_turn() const
{
    return turn_color;
}

const std::vector<const Move*>& Board::legal_moves() const
{
    return legal_moves_cache;
}

const std::vector<const Move*>& Board::other_moves() const
{
    return other_moves_cache;
}

void Board::ascii_draw(Color perspective) const
{
    const size_t square_width = 7;
    const size_t square_height = 3;

    const std::string square_corner = "+";
    const std::string square_horizontal_border = "-";
    const std::string square_vertical_border = "|";

    std::string horizontal_line;
    for(int i = 0; i < 8; ++i)
    {
        horizontal_line.append(square_corner);

        for(size_t j = 0; j < square_width; ++j)
        {
            horizontal_line.append(square_horizontal_border);
        }
    }
    horizontal_line.append(square_corner + "\n");
    const std::string left_spacer = "   ";

    int rank_start = (perspective == WHITE ? 8 : 1);
    int rank_end = (perspective == WHITE ? 1 : 8);
    int d_rank = (perspective == WHITE ? -1 : 1);
    char file_start = (perspective == WHITE ? 'a' : 'h');
    char file_end = (perspective == WHITE ? 'h' : 'a');
    int d_file = (perspective == WHITE ? 1 : -1);

    for(int rank = rank_start; d_rank*(rank_end - rank) >= 0; rank += d_rank)
    {
        std::cout << left_spacer << horizontal_line;

        for(size_t square_row = 0; square_row < square_height; ++square_row)
        {
            if(square_row == square_height/2)
            {
                std::cout << " " << rank << " ";
            }
            else
            {
                std::cout << left_spacer;
            }
            for(char file = file_start; d_file*(file_end - file) >= 0; file += d_file)
            {
                std::string piece_symbol;
                auto piece = piece_on_square(file, rank);
                char dark_square_fill = ':';
                char filler = (square_color(file, rank) == WHITE ? ' ' : dark_square_fill);
                if(piece)
                {
                    auto piece_row = piece->ascii_art(square_row);
                    std::string padding((square_width - piece_row.size())/2, filler);
                    piece_symbol = padding + piece_row + padding;
                }
                else
                {
                    piece_symbol.append(square_width, filler);
                }
                std::cout << square_vertical_border << piece_symbol;
            }
            std::cout << square_vertical_border << "\n";
        }
    }
    std::cout << left_spacer << horizontal_line;

    std::cout << left_spacer;
    for(char file = file_start; d_file*(file_end - file) >= 0; file += d_file)
    {
        std::cout << " " << std::string(square_width/2, ' ')
                  << file
                  << std::string(square_width/2, ' ');
    }
    std::cout << std::endl << std::endl;
}

void Board::remove_piece(char file, int rank)
{
    place_piece(nullptr, file, rank);
}

void Board::place_piece(const Piece* piece, char file, int rank)
{
    piece_on_square(file, rank) = piece;
    if(piece && piece->is_king())
    {
        king_location[piece->color()] = {file, rank};
    }
}

bool Board::king_is_in_check() const
{
    return ! checking_squares.empty();
}

bool Board::safe_for_king(char file, int rank, Color king_color) const
{
    auto attacking_color = opposite(king_color);
    auto king = get_king(king_color);
    int pawn_rank = rank - (attacking_color == WHITE ? 1 : -1); // which direction pawns attack

    // Straight-line moves
    for(auto rank_step : {-1, 0, 1})
    {
        for(auto file_step : {-1, 0, 1})
        {
            if(file_step == 0 && rank_step == 0)
            {
                continue;
            }

            for(int steps = 1; steps <= 7; ++steps)
            {
                char attacking_file = file + file_step*steps;
                int  attacking_rank = rank + rank_step*steps;

                if( ! inside_board(attacking_file, attacking_rank))
                {
                    break;
                }

                auto piece = piece_on_square(attacking_file, attacking_rank);
                if(( ! piece) || piece == king) // pretend king is not there when checking other squares
                {
                    continue;
                }

                if(piece->color() != attacking_color)
                {
                    break; // piece on square is blocking anything behind it
                }

                if(steps == 1 && piece->is_king())
                {
                    return false;
                }

                if(piece->is_queen())
                {
                    return false;
                }

                if(file_step == 0 || rank_step == 0)
                {
                    if(piece->is_rook())
                    {
                        return false;
                    }
                }
                else
                {
                    if(attacking_rank == pawn_rank && piece->is_pawn())
                    {
                        return false;
                    }

                    if(piece->is_bishop())
                    {
                        return false;
                    }
                }

                break; // piece on square is blocking anything behind it
            }
        }
    }

    // Check for knight attacks
    auto knight = get_knight(attacking_color);
    for(auto file_step : {1, 2})
    {
        auto rank_step = 3 - file_step;
        for(auto file_step_direction : {-1, 1})
        {
            for(auto rank_step_direction : {-1, 1})
            {
                char attacking_file = file + file_step*file_step_direction;
                char attacking_rank = rank + rank_step*rank_step_direction;

                if( ! inside_board(attacking_file, attacking_rank))
                {
                    continue;
                }

                if(piece_on_square(attacking_file, attacking_rank) == knight)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void Board::refresh_checking_squares()
{
    checking_squares.clear();
    auto king_square = king_location[whose_turn()];

    if(game_record.empty())
    {
        auto attacking_color = opposite(whose_turn());
        int pawn_rank = king_square.rank - (attacking_color == WHITE ? 1 : -1); // which direction pawns attack

        // Straight-line moves
        for(auto rank_step :{-1, 0, 1})
        {
            for(auto file_step :{-1, 0, 1})
            {
                if(file_step == 0 && rank_step == 0)
                {
                    continue;
                }

                for(int steps = 1; steps <= 7; ++steps)
                {
                    char attacking_file = king_square.file + file_step*steps;
                    int  attacking_rank = king_square.rank + rank_step*steps;

                    if( ! inside_board(attacking_file, attacking_rank))
                    {
                        break;
                    }

                    auto piece = piece_on_square(attacking_file, attacking_rank);
                    if( ! piece)
                    {
                        continue;
                    }

                    if(piece->color() != attacking_color)
                    {
                        break; // piece on square is blocking anything behind it
                    }

                    if(piece->is_queen())
                    {
                        checking_squares.push_back({attacking_file, attacking_rank});
                    }

                    if(file_step == 0 || rank_step == 0)
                    {
                        if(piece->is_rook())
                        {
                            checking_squares.push_back({attacking_file, attacking_rank});
                        }
                    }
                    else
                    {
                        if(attacking_rank == pawn_rank && piece->is_pawn())
                        {
                            checking_squares.push_back({attacking_file, attacking_rank});
                        }

                        if(piece->is_bishop())
                        {
                            checking_squares.push_back({attacking_file, attacking_rank});
                        }
                    }

                    break; // piece on square is blocking anything behind it
                }
            }
        }

        // Check for knight attacks
        auto knight = get_knight(attacking_color);
        for(auto file_step :{1, 2})
        {
            auto rank_step = 3 - file_step;
            for(auto file_step_direction :{-1, 1})
            {
                for(auto rank_step_direction :{-1, 1})
                {
                    char attacking_file = king_square.file + file_step*file_step_direction;
                    char attacking_rank = king_square.rank + rank_step*rank_step_direction;

                    if( ! inside_board(attacking_file, attacking_rank))
                    {
                        continue;
                    }

                    if(piece_on_square(attacking_file, attacking_rank) == knight)
                    {
                        checking_squares.push_back({attacking_file, attacking_rank});
                    }
                }
            }
        }
    }
    else
    {
        auto last_move = game_record.back();
        auto moved_piece = piece_on_square(last_move->end_file(), last_move->end_rank());

        // Moved piece now attacks king
        if(attacks(last_move->end_file(), last_move->end_rank(), king_square.file, king_square.rank))
        {
            checking_squares.push_back({last_move->end_file(), last_move->end_rank()});
        }
        
        
        // Discovered check
        if(auto pinning_square = piece_is_pinned(last_move->start_file(), last_move->start_rank()))
        {
            // Prevents pawn promotion that results in check from registering here as it would have
            // already been added in the previous if() block
            if(checking_squares.empty() || pinning_square != checking_squares.front())
            {
                checking_squares.push_back(pinning_square);
            }
        }

        // Discovered check due to en passant
        if(last_move->is_en_passant())
        {
            if(auto pinning_square = piece_is_pinned(last_move->end_file(), last_move->start_rank()))
            {
                checking_squares.push_back(pinning_square);
            }
        }

        // Discovered check by rook due to castling
        if(std::abs(last_move->file_change()) == 2 &&
           piece_on_square(last_move->end_file(), last_move->end_rank())->is_king())
        {
            char rook_file = (last_move->file_change() > 0 ? 'f' : 'd');
            if(attacks(rook_file, last_move->end_rank(), king_square.file, king_square.rank))
            {
                checking_squares.push_back({rook_file, last_move->end_rank()});
            }
        }
    }
}

bool Board::king_is_in_check_after_move(const Move& move) const
{
    if(piece_on_square(move.start_file(), move.start_rank())->is_king())
    {
        return ! safe_for_king(move.end_file(), move.end_rank(), whose_turn());
    }

    if(king_is_in_check())
    {
        // King is under check from multiple pieces but not moving
        if(king_multiply_checked())
        {
            return true;
        }

        // Checking piece is captured by non-pinned piece
        if(checking_squares.front() == Square{move.end_file(), move.end_rank()})
        {
            return piece_is_pinned(move.start_file(), move.start_rank());
        }

        // Non-pinned piece moves to block check
        if(auto blocking_square = piece_is_pinned(move.end_file(), move.end_rank()))
        {
            // Make sure piece being blocked is acutally doing the checking and the blocking piece can move
            return (blocking_square == checking_squares.front()) && piece_is_pinned(move.start_file(), move.start_rank());
        }

        // Nothing is done about check
        return true;
    }

    if(piece_is_pinned(move.start_file(), move.start_rank()))
    {
        if(piece_on_square(move.start_file(), move.start_rank())->is_knight())
        {
            return true; // knights cannot escape pins
        }

        // piece moves off line of attack to king
        auto king_square = king_location[whose_turn()];

        // Column move
        if(move.file_change() == 0)
        {
            return move.start_file() != king_square.file;
        }

        // Row move
        if(move.rank_change() == 0)
        {
            return move.start_rank() != king_square.rank;
        }

        // Move is diagonal, check if pin is on row or column
        if(move.start_file() == king_square.file || move.start_rank() == king_square.rank)
        {
            return true;
        }

        // check that diagonal moves are parallel to pin direction
        auto diagonal_of_move = (move.file_change() == move.rank_change());
        auto diagonal_of_piece_to_king = ((king_square.file - move.start_file()) == (king_square.rank - move.start_rank()));
        return diagonal_of_move != diagonal_of_piece_to_king;
    }

    // Make sure pawn captured en passant wasnt' blocking check
    if(move.is_en_passant())
    {
        // Captured pawn was blocking diagonal check
        if(piece_is_pinned(move.end_file(), move.start_rank()))
        {
            return king_location[whose_turn()].file != move.end_file();
        }

        // Check if king is on same row as both pawns and there is an enemy rook or queen on row
        auto king_square = king_location[whose_turn()];
        if(king_square.rank != move.start_rank())
        {
            return false;
        }

        auto step = (move.start_file() > king_square.file ? 1 : -1);
        auto attacking_file = move.start_file();
        auto attacked_file = move.end_file();
        auto passed_pawns = 0;
        for(char file = king_square.file + step; inside_board(file); file += step)
        {
            if(file == attacked_file || file == attacking_file)
            {
                ++passed_pawns;
                continue;
            }

            auto piece = piece_on_square(file, king_square.rank);
            if(piece)
            {
                if(passed_pawns < 2)
                {
                    break;
                }

                if(piece->color() == whose_turn())
                {
                    break;
                }

                if(piece->is_rook() || piece->is_queen())
                {
                    return true;
                }
                else
                {
                    break;
                }
            }
        }
    }

    return false;
}

bool Board::no_legal_moves() const
{
    return legal_moves().empty();
}

const std::vector<const Move*>& Board::get_game_record() const
{
    return game_record;
}

void Board::print_game_record(const Player* white,
                              const Player* black,
                              const std::string& file_name,
                              const Game_Result& result,
                              double initial_time,
                              size_t moves_to_reset,
                              double increment,
                              const Clock& game_clock) const
{
    static int game_number = 0;
    if(game_number == 0)
    {
        game_number = 1;
        std::ifstream ifs(file_name);
        std::string line;
        while(std::getline(ifs, line))
        {
            if(String::starts_with(line, "[Round"))
            {
                ++game_number;
            }
        }
    }

    std::ofstream ofs(file_name, std::ios::app);
    std::ostream& out_stream = (ofs ? ofs : std::cout);

    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    out_stream << "[Date \"" << std::put_time(std::localtime(&now_c), "%Y.%m.%d") << "\"]\n";
    out_stream << "[Time \"" << std::put_time(std::localtime(&now_c), "%H:%M:%S") << "\"]\n";

    if(white && ! white->name().empty())
    {
        out_stream << "[White \"" << white->name() << "\"]\n";
    }

    if(black && ! black->name().empty())
    {
        out_stream << "[Black \"" << black->name() << "\"]\n";
    }

    if(game_number > 0)
    {
        out_stream << "[Round \"" << game_number++ << "\"]\n";
    }

    if(initial_time > 0)
    {
        out_stream << "[TimeControl \"";
        if(moves_to_reset > 0)
        {
            out_stream << moves_to_reset << '/';
        }
        out_stream << initial_time;
        if(increment > 0)
        {
            out_stream << '+' << increment;
        }
        out_stream << "\"]\n";
    }

    if( ! result.get_game_ending_annotation().empty())
    {
        out_stream << "[Result \"" << result.get_game_ending_annotation() << "\"]\n";
    }

    if( ! result.get_ending_reason().empty() && ! String::contains(result.get_ending_reason(), "mates"))
    {
        out_stream << "[Termination \"" << result.get_ending_reason() << "\"]\n";
    }

    auto temp = Board();
    if( ! starting_fen.empty())
    {
        out_stream << "[SetUp \"1\"]\n";
        out_stream << "[FEN \"" << starting_fen << "\"]\n";
        temp = Board(starting_fen);
    }

    auto starting_turn_offset = (temp.whose_turn() == WHITE ? 2 : 3);

    for(size_t i = 0; i < game_record.size(); ++i)
    {
        if(temp.whose_turn() == WHITE || i == 0)
        {
            auto step = (i + starting_turn_offset)/2;
            out_stream << '\n' << move_count_start_offset + step << ".";
            if(i == 0 && temp.whose_turn() == BLACK)
            {
                out_stream << " ...";
            }
        }

        auto next_move = game_record.at(i);
        out_stream << " " << next_move->game_record_item(temp);

        std::string commentary;
        auto current_player = (temp.whose_turn() == WHITE ? white : black);
        if(current_player)
        {
            commentary = current_player->get_commentary_for_move(i/2);
        }

        if( ! commentary.empty())
        {
            auto comment_board = temp;
            out_stream << " { ";
            for(const auto& variation : String::split(commentary))
            {
                try
                {
                    const auto& comment_move = comment_board.get_move(variation);
                    out_stream << comment_move.game_record_item(comment_board) << " ";
                    comment_board.submit_move(comment_move);
                }
                catch(const Illegal_Move_Exception&)
                {
                    out_stream << variation << " ";
                }
            }
            out_stream << "}";
        }

        temp.submit_move(*next_move);
    }
    out_stream << '\n';

    if(initial_time > 0)
    {
        out_stream << "{ Time left: White: " << game_clock.time_left(WHITE) << " }\n"
                   << "{            Black: " << game_clock.time_left(BLACK) << " }\n\n";
    }
    out_stream << '\n';
}

std::string Board::board_status() const // for 3-fold rep count
{
    std::string s;

    int empty_count = 0;
    for(int rank = 8; rank >= 1; --rank)
    {
        if(rank < 8)
        {
            s.push_back('/');
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = piece_on_square(file, rank);
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s.push_back('0' + empty_count);
                    empty_count = 0;
                }
                s.push_back(piece->fen_symbol());
            }
        }
        if(empty_count > 0)
        {
            s.push_back('0' + empty_count);
            empty_count = 0;
        }
    }

    s.push_back(' ');
    s.push_back(whose_turn() == WHITE ? 'w' : 'b');
    s.push_back(' ');

    for(int base_rank : {1, 8})
    {
        if( ! piece_has_moved('e', base_rank)) // has king moved?
        {
            for(char rook_file : {'h', 'a'})
            {
                if( ! piece_has_moved(rook_file, base_rank)) // have rooks moved
                {
                    char mark = (rook_file == 'h' ? 'K' : 'Q');
                    if(base_rank == 8)
                    {
                        mark = std::tolower(mark);
                    }
                    s.push_back(mark);
                }
            }
        }
    }
    if(s.back() == ' ')
    {
        s.push_back('-');
    }
    s.push_back(' ');

    if(en_passant_target)
    {
        s.push_back(en_passant_target.file);
        s.push_back(en_passant_target.rank + '0');
    }
    else
    {
        s.push_back('-');
    }

    return s;
}

std::string Board::last_move_coordinates() const
{
    return game_record.back()->coordinate_move();
}

void Board::set_turn(Color color)
{
    if(turn_color != color)
    {
        clear_en_passant_target();
        switch_turn();
    }
}

void Board::switch_turn()
{
    turn_color = opposite(turn_color);
    recreate_move_caches();
}

void Board::make_en_passant_targetable(char file, int rank)
{
    en_passant_target = {file, rank};
}

bool Board::is_en_passant_targetable(char file, int rank) const
{
    return en_passant_target.file == file &&
           en_passant_target.rank == rank;
}

void Board::clear_en_passant_target()
{
    make_en_passant_targetable('\0', 0);
}

bool Board::piece_has_moved(char file, int rank) const
{
    return ! unmoved_positions[Board::board_index(file, rank)];
}

Square Board::find_king(Color color) const
{
    auto location = king_location[color];
    assert(piece_on_square(location.file, location.rank) == get_king(color));
    return location;
}

void Board::recreate_move_caches()
{
    other_moves_cache.clear();
    legal_moves_cache.clear();
    refresh_checking_squares();

    capturing_move_available = false;
    bool en_passant_legal = false;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = piece_on_square(file, rank);
            if(piece && piece->color() == whose_turn())
            {
                for(const auto& move : piece->get_move_list(file, rank))
                {
                    if(move->is_legal(*this))
                    {
                        legal_moves_cache.push_back(move);

                        if( ! en_passant_legal)
                        {
                            en_passant_legal = move->is_en_passant();
                        }

                        if( ! capturing_move_available)
                        {
                            capturing_move_available = (en_passant_legal || piece_on_square(move->end_file(), move->end_rank()));
                        }
                    }
                    else
                    {
                        other_moves_cache.push_back(move);
                    }
                }
            }
        }
    }

    if( ! en_passant_legal)
    {
        clear_en_passant_target();
    }
}

bool Board::enough_material_to_checkmate() const
{
    #ifdef DEBUG
        auto is_false = [](bool x){ return x == false; };
    #endif // DEBUG

    std::array<bool, 2> knight_found{};
    assert(std::all_of(knight_found.begin(), knight_found.end(), is_false));

    std::array<std::array<bool, 2>, 2> bishop_found{}; // bishop_found[Piece color][Square color]
    assert(std::all_of(bishop_found.begin(), bishop_found.end(),
                       [is_false](auto x){ return std::all_of(x.begin(), x.end(), is_false);}));

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = piece_on_square(file, rank);
            if( ! piece || piece->is_king())
            {
                continue;
            }
            else if(piece->is_queen() || piece->is_rook() || piece->is_pawn())
            {
                return true; // checkmate possible with just queen or rook; pawn can promote
            }
            else if(piece->is_knight())
            {
                if(knight_found[piece->color()])
                {
                    return true; // checkmate with two knights possible
                }
                else if(bishop_found[piece->color()][WHITE] || bishop_found[piece->color()][BLACK])
                {
                    return true; // checkmate with knight and bishop possible
                }
                knight_found[piece->color()] = true;
            }
            else if(piece->is_bishop())
            {
                if(knight_found[piece->color()])
                {
                    return true; // knight and bishop checkmate possible
                }

                auto bishop_square_color = square_color(file, rank);
                if(bishop_found[piece->color()][opposite(bishop_square_color)])
                {
                    return true; // checkmate with opposite colored bishops possible
                }
                bishop_found[piece->color()][bishop_square_color] = true;
            }

            // Checkmates when minor pieces block own king
            // KB......
            // ........ White king checkmated in corner
            // kn......
            // ........
            // ........
            // ........
            // ........
            // ........
            bool white_has_minor_pieces = (knight_found[WHITE] || bishop_found[WHITE][WHITE] || bishop_found[WHITE][BLACK]);
            bool black_has_minor_pieces = (knight_found[BLACK] || bishop_found[BLACK][WHITE] || bishop_found[BLACK][BLACK]);
            if(white_has_minor_pieces && black_has_minor_pieces)
            {
                if(knight_found[WHITE] || knight_found[BLACK])
                {
                    return true;
                }

                if(bishop_found[WHITE][WHITE] && bishop_found[BLACK][BLACK])
                {
                    return true;
                }

                if(bishop_found[WHITE][BLACK] && bishop_found[BLACK][WHITE])
                {
                    return true;
                }
            }
        }
    }

    return false;
}

std::string Board::get_last_move_record() const
{
    Board b;
    if( ! starting_fen.empty())
    {
        b = Board(starting_fen);
    }

    std::string result;
    for(const auto& move : get_game_record())
    {
        result = move->game_record_item(b);
        b.submit_move(*move);
    }

    return result;
}

void Board::set_thinking_mode(Thinking_Output_Type mode) const
{
    thinking_indicator = mode;
}

Thinking_Output_Type Board::get_thinking_mode() const
{
    return thinking_indicator;
}

Color Board::first_to_move() const
{
    return first_player_to_move;
}

bool Board::capture_possible() const
{
    return capturing_move_available;
}

bool Board::king_multiply_checked() const
{
    return checking_squares.size() > 1;
}

bool Board::all_empty_between(char file_start, int rank_start, char file_end, int rank_end) const
{
    if( ! straight_line_move(file_start, rank_start, file_end, rank_end))
    {
        return false;
    }

    auto file_step = Math::sign(file_end - file_start);
    auto rank_step = Math::sign(rank_end - rank_start);

    auto file = file_start + file_step;
    auto rank = rank_start + rank_step;

    while(file != file_end || rank != rank_end)
    {
        if(piece_on_square(file, rank))
        {
            return false;
        }

        file += file_step;
        rank += rank_step;
    }

    return true;
}

bool Board::straight_line_move(char file_start, int rank_start, char file_end, int rank_end)
{
    auto file_change = file_end - file_start;
    if(file_change == 0)
    {
        return true;
    }

    auto rank_change = rank_end - rank_start;
    if(rank_change == 0)
    {
        return true;
    }

    return std::abs(file_change) == std::abs(rank_change);
}

bool Board::attacks(char origin_file, int origin_rank, char target_file, int target_rank) const
{
    auto attacking_piece = piece_on_square(origin_file, origin_rank);
    if( ! attacking_piece)
    {
        return false;
    }

    auto file_change = target_file - origin_file;
    auto rank_change = target_rank - origin_rank;

    if(attacking_piece->is_knight())
    {
        return (std::abs(file_change) == 1 && std::abs(rank_change) == 2) ||
               (std::abs(file_change) == 2 && std::abs(rank_change) == 1);
    }

    if(attacking_piece->is_king())
    {
        return std::max(std::abs(file_change), std::abs(rank_change)) == 1;
    }

    if(attacking_piece->is_pawn())
    {
        return std::abs(file_change) == 1 && (rank_change == (attacking_piece->color() == WHITE ? 1 : -1));
    }

    if( ! all_empty_between(origin_file, origin_rank, target_file, target_rank))
    {
        return false;
    }

    if(file_change == 0 || rank_change == 0)
    {
        return attacking_piece->is_rook() || attacking_piece->is_queen();
    }

    if(std::abs(file_change) == std::abs(rank_change))
    {
        return attacking_piece->is_queen() || attacking_piece->is_bishop();
    }

    return false;
}

Square Board::piece_is_pinned(char file, int rank) const
{
    auto king_square = king_location[whose_turn()];
    auto no_pin = Square{};

    if(king_square == Square{file, rank})
    {
        return no_pin; // king is never pinned
    }

    if( ! straight_line_move(file, rank, king_square.file, king_square.rank))
    {
        return no_pin;
    }

    if( ! all_empty_between(king_square.file, king_square.rank, file, rank))
    {
        return no_pin;
    }

    auto file_change = file - king_square.file;
    auto file_step = Math::sign(file_change);
    auto rank_change = rank - king_square.rank;
    auto rank_step = Math::sign(rank_change);

    for(int step = 1; true; ++step) // loop until outside board or piece found
    {
        char current_file = file + file_step*step;
        int  current_rank = rank + rank_step*step;

        if( ! inside_board(current_file, current_rank))
        {
            return no_pin;
        }

        auto attacking_piece = piece_on_square(current_file, current_rank);
        if(attacking_piece)
        {
            if(attacking_piece->color() == whose_turn())
            {
                return no_pin;
            }

            if(attacking_piece->is_queen())
            {
                return {current_file, current_rank};
            }

            if(file_change == 0 || rank_change == 0)
            {
                if(attacking_piece->is_rook())
                {
                    return {current_file, current_rank};
                }
            }
            else
            {
                if(attacking_piece->is_bishop())
                {
                    return {current_file, current_rank};
                }
            }

            return no_pin;
        }
    }
}
