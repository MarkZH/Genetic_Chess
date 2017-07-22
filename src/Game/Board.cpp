#include <iostream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cassert>
#include <stdexcept>
#include <map>
#include <set>

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
    board(64, nullptr),
    turn_color(WHITE),
    en_passant_target({'\0', 0}),
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
        unmoved_positions.insert({'a', base_rank}); // Rook
        unmoved_positions.insert({'e', base_rank}); // King
        unmoved_positions.insert({'h', base_rank}); // Rook

        auto pawn_rank = (base_rank == 1 ? 2 : 7);
        for(char file = 'a'; file <= 'h'; ++file)
        {
            place_piece(get_pawn(color), file, pawn_rank);
        }
    }

    ++repeat_count[board_status()]; // Count initial position
}

Board::Board(const std::string& fen) :
    board(64, nullptr),
    turn_color(WHITE),
    en_passant_target({'\0', 0}),
    starting_fen(fen),
    thinking_indicator(NO_THINKING)
{
    auto fen_parse = String::split(fen);

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
                        place_piece(get_king(color), file, rank);
                        break;
                    default:
                        throw std::runtime_error(std::string("Invalid  symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }
    }

    turn_color = (fen_parse[1] == "w" ? WHITE : BLACK);

    auto castling_parse = fen_parse.at(2);
    if(String::contains(castling_parse, 'K'))
    {
        unmoved_positions.insert({'h', 1});
        unmoved_positions.insert({'e', 1});
    }
    if(String::contains(castling_parse, 'Q'))
    {
        unmoved_positions.insert({'a', 1});
        unmoved_positions.insert({'e', 1});
    }
    if(String::contains(castling_parse, 'k'))
    {
        unmoved_positions.insert({'h', 8});
        unmoved_positions.insert({'e', 8});
    }
    if(String::contains(castling_parse, 'q'))
    {
        unmoved_positions.insert({'a', 8});
        unmoved_positions.insert({'e', 8});
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

const Piece* Board::view_piece_on_square(char file, int rank) const
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
                if(move->name().back() == promote)
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

    set_turn(opposite(whose_turn()));
    move.side_effects(*this);

    if(no_legal_moves())
    {
        if(king_is_in_check(whose_turn())) // king in check
        {
            auto winner = opposite(whose_turn());
            return Game_Result(winner, color_text(winner) + " mates", false);
        }
        else
        {
            return Game_Result(NONE, "Stalemate", false);
        }
    }

    // Check if en passant is actually legal
    if(en_passant_target)
    {
        auto en_passant_legal = false;
        auto rank_origin = (en_passant_target.rank == 3 ? 4 : 5);
        for(auto file_origin : {en_passant_target.file - 1, en_passant_target.file + 1})
        {
            if( ! inside_board(file_origin))
            {
                continue;
            }

            auto piece = view_piece_on_square(file_origin, rank_origin);
            if(piece &&
               piece->color() == whose_turn() &&
               piece->is_pawn() &&
               is_legal(file_origin, rank_origin, en_passant_target.file, en_passant_target.rank))
            {
                en_passant_legal = true;
                break;
            }
        }

        if( ! en_passant_legal)
        {
            clear_en_passant_target();
        }
    }

    // An insufficient material draw can only happen after a capture,
    // which clears the repeat_count map.
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
    if(fifty_move_count >= 100) // "Move" means both players move.
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

    for(char file = file_search_start; file <= file_search_end; ++file)
    {
        for(int rank = rank_search_start; rank <= rank_search_end; ++rank)
        {
            auto piece = view_piece_on_square(file, rank);
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
                    throw Illegal_Move_Exception("Ambiguous move: " + move);
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

    unmoved_positions.erase({file_start, rank_start});
    unmoved_positions.erase({file_end, rank_end});

    clear_en_passant_target();
}

Color Board::whose_turn() const
{
    return turn_color;
}

const std::vector<const Move*>& Board::legal_moves() const
{
    if( ! legal_moves_cache.empty())
    {
        return legal_moves_cache;
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = view_piece_on_square(file, rank);
            if(piece && piece->color() == whose_turn())
            {
                for(const auto& move : piece->get_move_list(file, rank))
                {
                    if(move->is_legal(*this))
                    {
                        legal_moves_cache.push_back(move);
                    }
                    else
                    {
                        other_moves_cache.push_back(move);
                    }
                }
            }
        }
    }

    return legal_moves_cache;
}

const std::vector<const Move*>& Board::other_moves() const
{
    return other_moves_cache;
}

void Board::ascii_draw(Color perspective) const
{
    const int square_width = 7;
    const int square_height = 3;

    const std::string square_corner = "+";
    const std::string square_horizontal_border = "-";
    const std::string square_vertical_border = "|";

    std::string horizontal_line;
    for(int i = 0; i < 8; ++i)
    {
        horizontal_line.append(square_corner);

        for(int j = 0; j < square_width; ++j)
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

        for(int square_row = 0; square_row < square_height; ++square_row)
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
                auto piece = view_piece_on_square(file, rank);
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
}

bool Board::king_is_in_check(Color king_color) const
{
    // find king of input color
    auto king_square = find_king(king_color);
    return ! safe_for_king(king_square.file, king_square.rank, king_color);
}

bool Board::safe_for_king(char file, int rank, Color king_color) const
{
    auto attacking_color = opposite(king_color);
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

                auto piece = view_piece_on_square(attacking_file, attacking_rank);
                if( ! piece)
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

                auto piece = view_piece_on_square(attacking_file, attacking_rank);
                if(piece && piece->is_knight() && piece->color() == attacking_color)
                {
                    return false;
                }
            }
        }
    }

    return true;
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
                              Game_Result result,
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

    if( ! String::contains(result.get_ending_reason(), "mates"))
    {
        out_stream << "[Termination \"" << result.get_ending_reason() << "\"]\n";
    }

    auto temp = Board();
    auto move_count_start = 0;
    if( ! starting_fen.empty())
    {
        out_stream << "[SetUp \"1\"]\n";
        out_stream << "[FEN \"" << starting_fen << "\"]\n";
        temp = Board(starting_fen);
        move_count_start = std::stoi(String::split(starting_fen).back()) - 1;
    }

    for(size_t i = 0; i < game_record.size(); ++i)
    {
        if(temp.whose_turn() == WHITE || i == 0)
        {
            auto step = (i + 2)/2;
            out_stream << '\n' << move_count_start + step << ".";
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
    out_stream << std::endl;
}

std::string Board::board_status() const // for 3-fold rep count
{
    std::string s;

    int empty_count = 0;
    for(int rank = 8; rank >= 1; --rank)
    {
        if(rank < 8)
        {
            s.append("/");
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = view_piece_on_square(file, rank);
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s.append(std::to_string(empty_count));
                    empty_count = 0;
                }
                s.push_back(piece->fen_symbol());
            }
        }
        if(empty_count > 0)
        {
            s.append(std::to_string(empty_count));
            empty_count = 0;
        }
    }

    s.push_back(' ');
    s.push_back(tolower(color_text(whose_turn())[0]));
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
        clear_caches();
        clear_en_passant_target();
        turn_color = color;
    }
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
    en_passant_target = {'\0', 0};
}

bool Board::piece_has_moved(char file, int rank) const
{
    return unmoved_positions.count({file, rank}) == 0;
}

Square Board::find_king(Color color) const
{
    for(char king_file = 'a'; king_file <= 'h'; ++king_file)
    {
        for(int king_rank = 1; king_rank <= 8; ++king_rank)
        {
            auto piece = view_piece_on_square(king_file, king_rank);
            if(piece && piece->color() == color && piece->is_king())
            {
                return {king_file, king_rank};
            }
        }
    }

    ascii_draw(WHITE);
    throw std::runtime_error(color_text(color) + " king not found on board.");
}

void Board::clear_caches()
{
    other_moves_cache.clear();
    legal_moves_cache.clear();
}

bool Board::enough_material_to_checkmate() const
{
    std::map<Color, bool> knight_found;
    std::map<std::tuple<Color, Color>, bool> bishop_found; // <Piece, Square> color -> bishop found

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = view_piece_on_square(file, rank);
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
                else if(bishop_found[{piece->color(), WHITE}] || bishop_found[{piece->color(), BLACK}])
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
                if(bishop_found[{piece->color(), opposite(bishop_square_color)}])
                {
                    return true; // checkmate with opposite colored bishops possible
                }
                bishop_found[{piece->color(), bishop_square_color}] = true;
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
            bool white_has_minor_pieces = (knight_found[WHITE] || bishop_found[{WHITE,WHITE}] || bishop_found[{WHITE,BLACK}]);
            bool black_has_minor_pieces = (knight_found[BLACK] || bishop_found[{BLACK,WHITE}] || bishop_found[{BLACK,BLACK}]);
            if(white_has_minor_pieces && black_has_minor_pieces)
            {
                if(knight_found[WHITE] || knight_found[BLACK])
                {
                    return true;
                }

                if(bishop_found[{WHITE,WHITE}] && bishop_found[{BLACK, BLACK}])
                {
                    return true;
                }

                if(bishop_found[{WHITE,BLACK}] && bishop_found[{BLACK, WHITE}])
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
