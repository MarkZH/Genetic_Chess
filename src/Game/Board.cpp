#include <iostream>
#include <fstream>
#include <cctype>
#include <cassert>
#include <stdexcept>
#include <array>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <cmath>
#include <atomic>
#include <functional>
using namespace std::placeholders; // for _1, _2, etc. in std::bind()
#include <string>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"

#include "Moves/Move.h"
#include "Moves/Castle.h"

#include "Players/Player.h"

#include "Exceptions/Illegal_Move.h"

#include "Utility/Random.h"
#include "Utility/String.h"

namespace
{
    const auto square_hash_values =
    []()
    {
        // One entry for each piece on each square (including no piece)
        std::array<std::array<uint64_t, 13>, 64> hash_cache;
        for(auto& square_indexed_row : hash_cache)
        {
            std::generate(square_indexed_row.begin(),
                          square_indexed_row.end(),
                          Random::random_unsigned_int64);
        }
        return hash_cache;
    }();

    const auto en_passant_hash_values =
    []()
    {
        // One entry for each file
        std::array<uint64_t, 8> en_passant_hash_cache;
        std::generate(en_passant_hash_cache.begin(),
                      en_passant_hash_cache.end(),
                      Random::random_unsigned_int64);
        return en_passant_hash_cache;
    }();

    const auto castling_hash_values =
    []()
    {
        // One entry for each rook
        std::array<uint64_t, 4> castling_hash_cache;
        std::generate(castling_hash_cache.begin(),
                      castling_hash_cache.end(),
                      Random::random_unsigned_int64);
        return castling_hash_cache;
    }();

    const uint64_t switch_turn_board_hash = Random::random_unsigned_int64();

    const std::string standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::atomic<Thinking_Output_Type> thinking_indicator = Thinking_Output_Type::NO_THINKING;
    std::atomic_bool move_immediately = false;
}

Board::Board() noexcept : Board(standard_starting_fen)
{
}

Board::Board(const std::string& input_fen) : starting_fen(String::remove_extra_whitespace(input_fen))
{
    auto fen_parse = String::split(input_fen);
    if(fen_parse.size() != 6)
    {
        fen_error("Wrong number of fields (should be 6)");
    }

    auto board_parse = String::split(fen_parse.at(0), "/");
    if(board_parse.size() != 8)
    {
        fen_error("Board has wrong number of rows (should be 8)");
    }

    for(int rank = 8; rank >= 1; --rank)
    {
        char file = 'a';
        for(auto symbol : board_parse.at(size_t(8)-rank))
        {
            if(isdigit(symbol))
            {
                file += symbol - '0';
                if(file > 'h' + 1)
                {
                    fen_error("Too many squares in rank " + std::to_string(rank));
                }
            }
            else
            {
                if(file > 'h')
                {
                    fen_error("Too many squares in rank " + std::to_string(rank));
                }

                Color color = (isupper(symbol) ? WHITE : BLACK);
                switch(toupper(symbol))
                {
                    case 'P':
                        if(rank == 1 || rank == 8)
                        {
                            fen_error("Pawns cannot be placed on the home ranks.");
                        }
                        place_piece({color, PAWN}, {file, rank});
                        break;
                    case 'R':
                        place_piece({color, ROOK}, {file, rank});
                        break;
                    case 'N':
                        place_piece({color, KNIGHT}, {file, rank});
                        break;
                    case 'B':
                        place_piece({color, BISHOP}, {file, rank});
                        break;
                    case 'Q':
                        place_piece({color, QUEEN}, {file, rank});
                        break;
                    case 'K':
                        if(king_location[color].is_set())
                        {
                            fen_error("More than one " + color_text(color) + " king.");
                        }
                        place_piece({color, KING}, {file, rank});
                        break;
                    default:
                        fen_error(std::string("Invalid symbol in FEN string: ") + symbol);
                }
                ++file;
            }
        }

        if(file != 'h' + 1)
        {
            fen_error("Too few squares in rank " + std::to_string(rank));
        }
    }

    if( ! find_king(WHITE).is_set())
    {
        fen_error("White king not in FEN string");
    }

    if( ! find_king(BLACK).is_set())
    {
        fen_error("Black king not in FEN string");
    }

    if(fen_parse[1] == "w")
    {
        turn_color = WHITE;
    }
    else if(fen_parse[1] == "b")
    {
        turn_color = BLACK;
    }
    else
    {
        fen_error("Invalid character for whose turn: " + fen_parse[1]);
    }

    auto non_turn_color = opposite(whose_turn());
    if( ! safe_for_king(find_king(non_turn_color), non_turn_color))
    {
        fen_error(color_text(non_turn_color) +
                       " is in check but it is " +
                       color_text(whose_turn()) + "'s turn.");
    }

    auto castling_parse = fen_parse.at(2);
    if(castling_parse != "-")
    {
        if(String::contains(castling_parse, '-'))
        {
            fen_error("Castling section contains - and other characters: " + castling_parse + ".");
        }

        for(auto c : castling_parse)
        {
            if( ! String::contains("KQkq", c))
            {
                fen_error(std::string("Illegal character in castling section: ") + c + "(" + castling_parse + ")");
            }

            Color piece_color = std::isupper(c) ? WHITE : BLACK;
            auto rook_square = Square{std::toupper(c) == 'K' ? 'h' : 'a', std::isupper(c) ? 1 : 8};
            auto king_square = Square{'e', rook_square.rank()};
            std::string side = std::toupper(c) == 'K' ? "king" : "queen";

            if(piece_on_square(rook_square) != Piece{piece_color, ROOK})
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " rook on " + rook_square.string() + " to castle " + side + "side.");
            }
            set_unmoved(rook_square);

            if(piece_on_square(king_square) != Piece{piece_color, KING})
            {
                fen_error("There must be a " + String::lowercase(color_text(piece_color)) + " king on " + king_square.string() + " to castle.");
            }
            set_unmoved(king_square);
        }
    }

    auto en_passant_parse = fen_parse.at(3);
    if(en_passant_parse != "-")
    {
        if(en_passant_parse.size() != 2)
        {
            fen_error("Invalid en passant square.");
        }

        make_en_passant_targetable({en_passant_parse[0], en_passant_parse[1] - '0'});

        if( ! en_passant_target.is_set())
        {
            fen_error("Invalid en passant square.");
        }

        if(piece_on_square(en_passant_target))
        {
            fen_error("Piece is not allowed on en passant target square.");
        }

        auto last_move_pawn = piece_on_square(en_passant_target + Square_Difference{0, whose_turn() == WHITE ? -1 : 1});;
        if(last_move_pawn != Piece{opposite(whose_turn()), PAWN})
        {
            fen_error("There must be a pawn past the en passant target square.");
        }
    }

    // Fill repeat counter to indicate moves since last
    // pawn move or capture.
    auto fifty_move_count_input = String::string_to_size_t(fen_parse.at(4));
    if(fifty_move_count_input > 100)
    {
        fen_error("Halfmove clock value too large.");
    }
    add_board_position_to_repeat_record();
    while(moves_since_pawn_or_capture() < fifty_move_count_input)
    {
        add_to_repeat_count(Random::random_unsigned_int64());
    }

    move_count_start_offset = String::string_to_size_t(fen_parse.at(5));

    if(fen() != starting_fen)
    {
        fen_error("Result: " + fen());
    }

    const auto& king_square = find_king(whose_turn());
    const auto& attacks_on_king = moves_attacking_square(king_square, opposite(whose_turn()));
    if(attacks_on_king.count() > 2)
    {
        fen_error("Too many pieces attacking " + color_text(whose_turn()) + " king.");
    }

    // Count knight attacks
    if((attacks_on_king >> 8).count() > 1)
    {
        fen_error("It is impossible for more than one knight to check king.");
    }

    recreate_move_caches();

    // In case a listed en passant target is not actually a legal move.
    --repeat_count_insertion_point;
    add_board_position_to_repeat_record();
}

void Board::fen_error(const std::string& reason) const
{
    throw std::invalid_argument("Bad FEN input: " + starting_fen + "\n" + reason);
}

Piece& Board::piece_on_square(Square square) noexcept
{
    return board[square.index()];
}

Piece Board::piece_on_square(Square square) const noexcept
{
    return board[square.index()];
}

void Board::set_unmoved(Square square) noexcept
{
    update_board_hash(square); // remove reference to moved piece
    unmoved_positions[square.index()] = true;
    update_board_hash(square);
}

bool Board::is_legal(Square start, Square end) const noexcept
{
    return std::any_of(legal_moves().begin(), legal_moves().end(),
                       [&](auto move)
                       {
                           return move->start() == start &&
                                  move->end() == end;
                       });
}

bool Board::is_in_legal_moves_list(const Move& move) const noexcept
{
    return std::find(legal_moves().begin(), legal_moves().end(), &move) != legal_moves().end();
}

std::string Board::fen() const noexcept
{
    std::string s;

    for(int rank = 8; rank >= 1; --rank)
    {
        int empty_count = 0;
        if(rank < 8)
        {
            s.push_back('/');
        }

        for(char file = 'a'; file <= 'h'; ++file)
        {
            auto piece = piece_on_square({file, rank});
            if( ! piece)
            {
                ++empty_count;
            }
            else
            {
                if(empty_count > 0)
                {
                    s += std::to_string(empty_count);
                    empty_count = 0;
                }
                s.push_back(piece.fen_symbol());
            }
        }

        if(empty_count > 0)
        {
            s += std::to_string(empty_count);
        }
    }

    s.push_back(' ');
    s.push_back(whose_turn() == WHITE ? 'w' : 'b');
    s.push_back(' ');

    for(int base_rank : {1, 8})
    {
        if( ! piece_has_moved({'e', base_rank})) // has king moved?
        {
            for(char rook_file : {'h', 'a'})
            {
                if( ! piece_has_moved({rook_file, base_rank})) // has rook moved?
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

    if(en_passant_target.is_set())
    {
        s += en_passant_target.string();
    }
    else
    {
        s.push_back('-');
    }

    return s + " " +
        std::to_string(moves_since_pawn_or_capture()) + " " +
        std::to_string(move_count_start_offset + game_length()/2);
}

std::string Board::original_fen() const noexcept
{
    return starting_fen;
}

const Move& Board::create_move(Square start, Square end, char promote) const
{
    std::vector<const Move*> move_list;
    std::copy_if(legal_moves().begin(), legal_moves().end(), std::back_inserter(move_list),
                 [start, end](auto move)
                 {
                     return move->start() == start && move->end() == end;
                 });

    if(move_list.empty())
    {
        throw Illegal_Move("No legal move found for " + start.string() + end.string() + promote);
    }

    if(move_list.size() == 1)
    {
        return *move_list.front();
    }
    else
    {
        auto result = std::find_if(move_list.begin(), move_list.end(),
                                   [promote](auto move)
                                   {
                                       return move->promotion_piece_symbol() == std::toupper(promote);
                                   });

        if(result == move_list.end())
        {
            throw Illegal_Move("Invalid choice for pawn promotion (" + start.string() + end.string() + ")");
        }
        else
        {
            return **result; // one star to dereference the iterator, one to dereference the pointer
        }
    }
}

Game_Result Board::submit_move(const Move& move) noexcept
{
    update_board(move);
    return move_result();
}

Game_Result Board::submit_move(const std::string& move)
{
    return submit_move(create_move(move));
}

std::vector<const Move*> Board::derive_moves(const std::string& new_fen) const noexcept
{
    constexpr auto moves_so_far = [](const auto& b)
                                  {
                                      return 2*std::stoi(String::split(b.fen()).back()) +
                                             (b.whose_turn() == WHITE ? 0 : 1);
                                  };
    auto new_board = Board(new_fen);
    auto goal_fen = new_board.fen();
    auto moves_to_derive_count = moves_so_far(new_board) - moves_so_far(*this);
    if(moves_to_derive_count > 2 || moves_to_derive_count < 1)
    {
        return {};
    }

    for(auto first_move : legal_moves())
    {
        auto first_move_board = *this;
        first_move_board.submit_move(*first_move);
        if(moves_to_derive_count == 1)
        {
            if(first_move_board.fen() == goal_fen)
            {
                return {first_move};
            }
        }
        else
        {
            for(auto second_move : first_move_board.legal_moves())
            {
                auto second_move_board = first_move_board;
                second_move_board.submit_move(*second_move);
                if(second_move_board.fen() == goal_fen)
                {
                    return {first_move, second_move};
                }
            }
        }
    }

    return {};
}

Game_Result Board::move_result() const noexcept
{
    if(no_legal_moves())
    {
        if(king_is_in_check())
        {
            return Game_Result(opposite(whose_turn()), Game_Result_Type::CHECKMATE);
        }
        else
        {
            return Game_Result(NONE, Game_Result_Type::STALEMATE);
        }
    }

    // An insufficient material draw can only happen after a capture
    // or a pawn promotion to a minor piece, both of which clear the
    // repeat_count tracker.
    if(moves_since_pawn_or_capture() == 0 && ! enough_material_to_checkmate())
    {
        return Game_Result(NONE, Game_Result_Type::INSUFFICIENT_MATERIAL);
    }

    if(current_board_position_repeat_count() >= 3)
    {
        return Game_Result(NONE, Game_Result_Type::THREEFOLD_REPETITION);
    }

    // "Move" means both players move, so the fifty-move rule is
    // triggered after 100 player moves
    if(moves_since_pawn_or_capture() >= 100)
    {
        return Game_Result(NONE, Game_Result_Type::FIFTY_MOVE);
    }

    return {};
}

void Board::update_board(const Move& move) noexcept
{
    assert(is_in_legal_moves_list(move));

    ++game_move_count;
    previous_move = &move;
    move_piece(move);
    move.side_effects(*this);

    turn_color = opposite(turn_color);
    update_whose_turn_hash();

    recreate_move_caches();

    add_board_position_to_repeat_record();
}

size_t Board::game_length() const noexcept
{
    return game_move_count;
}

const Move* Board::last_move() const noexcept
{
    return previous_move;
}

int Board::castling_direction(Color player) const noexcept
{
    return castling_movement[player];
}

const Move& Board::create_move(const std::string& move) const
{
    static const std::string promotion_pieces = "RNBQK";
    static const std::string pieces = "P" + promotion_pieces;
    static const std::string lowercase_pieces = String::lowercase(pieces);
    static const std::string valid_files = "abcdefgh";
    static const std::string valid_rows  = "12345678";
    static const std::string castling = "Oo";
    static const std::string valid_characters = lowercase_pieces + pieces + valid_files + valid_rows + castling;

    std::string validated;
    std::copy_if(move.begin(), move.end(), std::back_inserter(validated),
                 [](auto c) { return String::contains(valid_characters, c); });

    if(validated.size() < 2)
    {
        throw Illegal_Move(move + " does not specify a valid move.");
    }

    // Castling
    if(String::lowercase(validated) == "oo")
    {
        auto king_square = find_king(whose_turn());
        return create_move(king_square,
                           king_square + Square_Difference{2, 0});
    }

    if(String::lowercase(validated) == "ooo")
    {
        auto king_square = find_king(whose_turn());
        return create_move(king_square,
                           king_square + Square_Difference{-2, 0});
    }

    // Capitalize piece symbols when unambiguous (i.e., not a bishop)
    for(auto& c : validated)
    {
        if(String::contains(lowercase_pieces, c) && c != 'b')
        {
            c = std::toupper(c);
        }
    }

    // Capitalize promotion piece symbol
    if(std::isalpha(validated.back()))
    {
        validated.back() = std::toupper(validated.back());
    }

    std::string moving_pieces;
    std::copy_if(validated.begin(), validated.end(), std::back_inserter(moving_pieces),
                 [](auto c) { return std::isupper(c); });
    std::string files;
    std::copy_if(validated.begin(), validated.end(), std::back_inserter(files),
                 [](auto c) { return std::islower(c); });
    std::string ranks;
    std::copy_if(validated.begin(), validated.end(), std::back_inserter(ranks),
                 [](auto c) { return std::isdigit(c); });

    if(files.empty() || ranks.empty())
    {
        throw Illegal_Move("Destination square not specified: " + move);
    }
    else if(files.size() > 2 || ranks.size() > 2)
    {
        throw Illegal_Move("Invalid move specification: " + move);
    }

    auto ending_square = Square{files.back(), ranks.back() - '0'};

    char starting_file = files.size() == 2 ? files.front() : '\0';
    int  starting_rank = ranks.size() > 1 ? ranks.front() - '0' : 0;

    std::string piece_symbol;
    char promoted_piece = '\0';
    if(moving_pieces.size() > 2)
    {
        throw Illegal_Move("Too many pieces mentioned: " + move);
    }
    else if(moving_pieces.size() == 2)
    {
        if(moving_pieces.front() != 'P')
        {
            throw Illegal_Move("Only pawns can be promoted: " + move);
        }

        if( ! String::contains(promotion_pieces, moving_pieces.back()))
        {
            throw Illegal_Move("Cannot promote to pawn: " + move);
        }
        promoted_piece = moving_pieces.back();
    }
    else if(moving_pieces.size() == 1)
    {
        if(String::starts_with(validated, moving_pieces))
        {
            if(validated.front() != 'P') // leave blank if pawn is moving
            {
                piece_symbol = moving_pieces.front();
            }
        }
        else if(String::ends_with(validated, moving_pieces))
        {
            promoted_piece = moving_pieces.front();
        }
        else
        {
            throw Illegal_Move("Invalid piece location: " + move);
        }
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
                auto square = Square{file, rank};
                auto piece = piece_on_square(square);
                if(piece &&
                   piece.pgn_symbol() == piece_symbol &&
                   is_legal(square, ending_square))
                {
                    if(starting_file == 0 || starting_rank == 0)
                    {
                        starting_file = file;
                        starting_rank = rank;
                    }
                    else
                    {
                        // If two moves satisfy text, argument is ambiguous.
                        throw Illegal_Move("Ambiguous move: " + move);
                    }
                }
            }
        }
    }

    if(starting_file != 0 && starting_rank != 0)
    {
        return create_move({starting_file, starting_rank},
                           ending_square,
                           promoted_piece);
    }

    throw Illegal_Move("Malformed move: " + move);
}

void Board::move_piece(const Move& move) noexcept
{
    if(piece_on_square(move.end()))
    {
        remove_piece(move.end());
        clear_repeat_count();
    }

    auto moving_piece = piece_on_square(move.start());
    if(moving_piece.type() == PAWN)
    {
        clear_repeat_count();
    }
    remove_piece(move.start());
    place_piece(moving_piece, move.end());

    clear_en_passant_target();
}

Color Board::whose_turn() const noexcept
{
    return turn_color;
}

const std::vector<const Move*>& Board::legal_moves() const noexcept
{
    return legal_moves_cache;
}

void Board::ascii_draw(Color perspective) const noexcept
{
    const size_t square_width = 7;
    const size_t square_height = 3;

    const std::string square_corner = "+";
    const std::string square_horizontal_border = "-";
    const std::string square_vertical_border = "|";

    std::string horizontal_line;
    for(size_t i = 0; i < 8; ++i)
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
    int d_rank = (perspective == WHITE ? -1 : 1);
    char file_start = (perspective == WHITE ? 'a' : 'h');
    int d_file = (perspective == WHITE ? 1 : -1);

    for(int rank = rank_start; Square::inside_board(rank); rank += d_rank)
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
            for(char file = file_start; Square::inside_board(file); file += d_file)
            {
                auto square = Square{file, rank};
                std::string piece_symbol;
                auto piece = piece_on_square(square);
                char filler = square.color() == WHITE ? ' ' : ':';
                if(piece)
                {
                    auto piece_row = piece.ascii_art(square_row, square_height);
                    std::string padding((square_width - piece_row.size())/2, filler);
                    piece_symbol = padding + piece_row + padding;
                    while(piece_symbol.size() < square_width)
                    {
                        piece_symbol.push_back(filler);
                    }
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
    for(char file = file_start; Square::inside_board(file); file += d_file)
    {
        std::cout << " " << std::string(square_width/2 - 1 + square_width%2, ' ')
                  << file
                  << std::string(square_width/2, ' ');
    }
    std::cout << std::endl << std::endl;
}

void Board::remove_piece(Square square) noexcept
{
    place_piece({}, square);
}

void Board::place_piece(Piece piece, Square square) noexcept
{
    update_board_hash(square); // XOR out piece on square

    auto old_piece = piece_on_square(square);
    piece_on_square(square) = piece;

    remove_attacks_from(square, old_piece);
    update_blocks(square, old_piece, piece);
    add_attacks_from(square, piece);

    auto update_rook_hashes = old_piece && old_piece.type() == KING && unmoved_positions[square.index()];
    if(update_rook_hashes)
    {
        // XOR out castling rights on rook squares
        update_board_hash({'a', square.rank()});
        update_board_hash({'h', square.rank()});
    }

    unmoved_positions[square.index()] = false;

    if(update_rook_hashes)
    {
        // XOR in hashes with no castling rights
        update_board_hash({'a', square.rank()});
        update_board_hash({'h', square.rank()});
    }

    update_board_hash(square); // XOR in new piece on square

    if(piece && piece.type() == KING)
    {
        king_location[piece.color()] = square;
    }
}

void Board::add_attacks_from(Square square, Piece piece) noexcept
{
    modify_attacks(square, piece, true);
}

void Board::modify_attacks(Square square, Piece piece, bool adding_attacks) noexcept
{
    if( ! piece)
    {
        return;
    }

    auto attacking_color = piece.color();
    auto vulnerable_king = Piece{opposite(attacking_color), KING};
    for(const auto& attack_move_list : piece.attacking_move_lists(square))
    {
        bool move_blocked = false;
        for(auto attack : attack_move_list)
        {
            auto attacked_square = attack->end();
            auto attacked_index = attacked_square.index();

            if(move_blocked)
            {
                blocked_attacks[attacking_color][attacked_index][attack->attack_index()] = adding_attacks;
            }
            else
            {
                auto blocking_piece = piece_on_square(attacked_square);

                potential_attacks[attacking_color][attacked_index][attack->attack_index()] = adding_attacks;

                if(blocking_piece && blocking_piece != vulnerable_king)
                {
                    move_blocked = true;
                }
            }
        }
    }
}

void Board::remove_attacks_from(Square square, Piece old_piece) noexcept
{
    modify_attacks(square, old_piece, false);
}

void Board::update_blocks(Square square, Piece old_piece, Piece new_piece) noexcept
{
    // Replacing nothing with nothing changes nothing.
    // Replacing one piece with another does not change which
    // moves are blocked. Only happens during pawn promotions.
    if(bool(old_piece) == bool(new_piece))
    {
        return;
    }

    auto add_new_attacks = ! new_piece; // New pieces block; no new pieces allow new moves through
    auto origin_square_index = square.index();

    for(auto attacking_color : {WHITE, BLACK})
    {
        auto vulnerable_king = Piece{opposite(attacking_color), KING};
        if(new_piece == vulnerable_king)
        {
            continue;
        }

        const auto& attack_direction_list = potential_attacks[attacking_color][origin_square_index];
        for(size_t index = 0; index < attack_direction_list.size()/2; ++index) // /2 to exclude knight moves, which are never blocked
        {
            if(attack_direction_list[index])
            {
                auto step = Move::attack_direction_from_index(index);
                auto revealed_attacker = piece_on_square(square - step);
                if(revealed_attacker && (revealed_attacker.type() == PAWN || revealed_attacker.type() == KING))
                {
                    continue; // Pawns and kings are never blocked
                }

                for(auto target_square : Square::square_line_from(square, step))
                {
                    auto target_index = target_square.index();
                    auto piece = piece_on_square(target_square);

                    potential_attacks[attacking_color][target_index][index] = add_new_attacks;
                    blocked_attacks[attacking_color][target_index][index] = ! add_new_attacks;

                    if(piece && piece != vulnerable_king)
                    {
                        break;
                    }
                }
            }
        }
    }
}

const std::bitset<16>& Board::moves_attacking_square(Square square, Color attacking_color) const noexcept
{
    return potential_attacks[attacking_color][square.index()];
}

const std::bitset<16>& Board::checking_moves() const noexcept
{
    return moves_attacking_square(find_king(whose_turn()), opposite(whose_turn()));
}

bool Board::king_is_in_check() const noexcept
{
    return checking_moves().any();
}

bool Board::safe_for_king(Square square, Color king_color) const noexcept
{
    return moves_attacking_square(square, opposite(king_color)).none();
}

bool Board::blocked_attack(Square square, Color attacking_color) const noexcept
{
    return blocked_attacks[attacking_color][square.index()].any();
}

bool Board::king_is_in_check_after_move(const Move& move) const noexcept
{
    auto king_square = find_king(whose_turn());
    if(move.start() == king_square)
    {
        return ! safe_for_king(move.end(), whose_turn());
    }

    if(king_is_in_check())
    {
        if(king_multiply_checked())
        {
            return true;
        }

        assert(checking_square.is_set());
        if(in_line_in_order(checking_square, move.end(), king_square))
        {
            return piece_is_pinned(move.start());
        }

        if(move.is_en_passant())
        {
            auto captured_pawn_square = Square{move.end().file(), move.start().rank()};
            return checking_square != captured_pawn_square || piece_is_pinned(move.start());
        }

        // Nothing is done about the check
        return true;
    }

    if(piece_is_pinned(move.start()))
    {
        return ! moves_are_parallel(move.movement(), king_square - move.start());
    }

    if(move.is_en_passant())
    {
        if(king_square.rank() != move.start().rank())
        {
            return false;
        }

        auto squares = Square::square_line_from(king_square, (move.start() - king_square).step());
        auto revealed_attacker = std::find_if(squares.begin(), squares.end(),
                                              [this](auto square)
                                              {
                                                  auto piece = piece_on_square(square);
                                                  return piece && piece.color() == opposite(whose_turn()) &&
                                                      (piece.type() == QUEEN || piece.type() == ROOK);
                                              });

        return revealed_attacker != squares.end() &&
               in_line_in_order(king_square, move.start(), *revealed_attacker) &&
               std::count_if(squares.begin(), revealed_attacker, [this](auto square) { return piece_on_square(square); }) == 2;
    }

    return false;
}

bool Board::no_legal_moves() const noexcept
{
    return legal_moves().empty();
}

template<typename OutputStream, typename DataType>
void print_game_header_line(OutputStream& output, const std::string& heading, const DataType& data)
{
    output << "[" << heading << " \"" << data << "\"]\n";
}

void Board::print_game_record(const std::vector<const Move*>& game_record_listing,
                              const Player* white,
                              const Player* black,
                              const std::string& file_name,
                              const Game_Result& result,
                              const Clock& game_clock,
                              const std::string& unusual_ending_reason) const noexcept
{
    static std::mutex write_lock;
    auto write_lock_guard = std::lock_guard(write_lock);

    static int game_number = 0;
    static std::string last_used_file_name;
    if(game_number == 0 || file_name != last_used_file_name)
    {
        game_number = 1;
        last_used_file_name = file_name;
        std::ifstream ifs(file_name);
        std::string line;
        while(std::getline(ifs, line))
        {
            if(String::starts_with(line, "[Round"))
            {
                auto round_number = std::stoi(String::split(line, "\"").at(1));
                if(round_number >= game_number)
                {
                    game_number = round_number + 1;
                }
            }
        }
    }

    std::ofstream ofs(file_name, std::ios::app);
    std::ostream& out_stream = (ofs ? ofs : std::cout);

    print_game_header_line(out_stream, "Event", "?");
    print_game_header_line(out_stream, "Site", "?");
    print_game_header_line(out_stream, "Date", String::date_and_time_format(game_clock.game_start_date_and_time(), "%Y.%m.%d"));
    print_game_header_line(out_stream, "Round", game_number++);

    for(auto player_color : {WHITE, BLACK})
    {
        auto player = player_color == WHITE ? white : black;
        auto other_player = player_color == WHITE ? black : white;
        if(player && ! player->name().empty())
        {
            print_game_header_line(out_stream, color_text(player_color), player->name());
        }
        else if(other_player && ! other_player->opponent_name().empty())
        {
            print_game_header_line(out_stream, color_text(player_color), other_player->opponent_name());
        }
        else
        {
            print_game_header_line(out_stream, color_text(player_color), "?");
        }
    }

    // Get actual result
    auto last_move_result = move_result();
    auto actual_result = last_move_result.game_has_ended() ? last_move_result : result;

    print_game_header_line(out_stream, "Result", actual_result.game_ending_annotation());

    print_game_header_line(out_stream, "Time", String::date_and_time_format(game_clock.game_start_date_and_time(), "%H:%M:%S"));

    if(game_clock.is_in_use())
    {
        std::ostringstream time_control_spec;
        if(game_clock.moves_per_time_period() > 0)
        {
            time_control_spec << game_clock.moves_per_time_period() << '/';
        }
        time_control_spec << game_clock.initial_time();
        if(game_clock.increment(WHITE) > 0)
        {
            time_control_spec << '+' << game_clock.increment(WHITE);
        }
        print_game_header_line(out_stream, "TimeControl", time_control_spec.str());
        print_game_header_line(out_stream, "TimeLeftWhite", game_clock.time_left(WHITE));
        print_game_header_line(out_stream, "TimeLeftBlack", game_clock.time_left(BLACK));
    }

    if( ! actual_result.ending_reason().empty() && ! String::contains(actual_result.ending_reason(), "mates"))
    {
        print_game_header_line(out_stream, "Termination", actual_result.ending_reason());
    }
    else if( ! actual_result.game_has_ended() && ! unusual_ending_reason.empty())
    {
        print_game_header_line(out_stream, "Termination", unusual_ending_reason);
    }

    auto commentary_board = Board(starting_fen);
    if(starting_fen != standard_starting_fen)
    {
        print_game_header_line(out_stream, "SetUp", 1);
        print_game_header_line(out_stream, "FEN", starting_fen);
    }

    auto starting_turn_offset = size_t(commentary_board.whose_turn() == WHITE ? 0 : 1);

    for(size_t i = 0; i < game_record_listing.size(); ++i)
    {
        auto step = move_count_start_offset + (i + starting_turn_offset)/2;
        if(commentary_board.whose_turn() == WHITE || i == 0)
        {
            out_stream << '\n' << step << ".";
            if(i == 0 && commentary_board.whose_turn() == BLACK)
            {
                out_stream << " ...";
            }
        }

        auto next_move = game_record_listing.at(i);
        out_stream << " " << next_move->game_record_item(commentary_board);
        auto current_player = (commentary_board.whose_turn() == WHITE ? white : black);
        if(current_player)
        {
            auto commentary = String::trim_outer_whitespace(current_player->commentary_for_next_move(commentary_board, step));
            if( ! commentary.empty())
            {
                out_stream << " " << commentary;
            }
        }
        commentary_board.submit_move(*next_move);
    }
    out_stream << " " << actual_result.game_ending_annotation() << "\n\n\n";

    assert(commentary_board.fen() == fen());
}

void Board::make_en_passant_targetable(Square square) noexcept
{
    if(en_passant_target.is_set())
    {
        update_board_hash(en_passant_target); // XOR out previous en passant target
    }

    en_passant_target = square;

    if(square.is_set())
    {
        update_board_hash(square);
    }
}

bool Board::is_en_passant_targetable(Square square) const noexcept
{
    return en_passant_target == square;
}

void Board::clear_en_passant_target() noexcept
{
    make_en_passant_targetable({});
}

bool Board::piece_has_moved(Square square) const noexcept
{
    return ! unmoved_positions[square.index()];
}

Square Board::find_king(Color color) const noexcept
{
    assert(piece_on_square(king_location[color]) == Piece(color, KING));
    return king_location[color];
}

void Board::recreate_move_caches() noexcept
{
    last_pin_check_square = Square{};
    checking_square = king_is_in_check() ? find_checking_square() : Square{};
    prior_moves_count = legal_moves_cache.size();
    legal_moves_cache.clear();
    for(auto square : Square::all_squares())
    {
        auto piece = piece_on_square(square);
        if(piece && piece.color() == whose_turn())
        {
            for(const auto& move_list : piece.move_lists(square))
            {
                for(auto move : move_list)
                {
                    if(move->is_legal(*this))
                    {
                        legal_moves_cache.push_back(move);
                    }

                    if(piece_on_square(move->end()))
                    {
                        break;
                    }
                }
            }
        }
    }

    if(std::none_of(legal_moves_cache.begin(),
                    legal_moves_cache.end(),
                    [](auto move) { return move->is_en_passant(); }))
    {
        clear_en_passant_target();
    }
}

Square Board::find_checking_square() const noexcept
{
    const auto& checks = checking_moves();
    size_t checking_index = 0;
    while( ! checks[checking_index])
    {
        ++checking_index;
    }
    auto step = Move::attack_direction_from_index(checking_index);
    const auto& king_square = find_king(whose_turn());
    auto squares = Square::square_line_from(king_square, -step);
    return *std::find_if(squares.begin(), squares.end(), [this](auto square) { return piece_on_square(square); });
}

bool Board::enough_material_to_checkmate(Color color) const noexcept
{
    auto piece_is_right_color = [color](auto piece) { return piece && (color == NONE || piece.color() == color); };

    if(std::any_of(board.begin(), board.end(),
                   [piece_is_right_color](auto piece)
                   {
                       return piece_is_right_color(piece) &&
                              (piece.type() == QUEEN || piece.type() == ROOK || piece.type() == PAWN);
                   }))
    {
        return true;
    }

    auto bishop_on_square_color =
        [this, piece_is_right_color](Color color_sought, Square square)
        {
            auto piece = piece_on_square(square);
            return piece_is_right_color(piece) && piece.type() == BISHOP && square.color() == color_sought;
        };

    auto squares = Square::all_squares();
    auto bishops_on_white = std::any_of(squares.begin(), squares.end(), std::bind(bishop_on_square_color, WHITE, _1));
    auto bishops_on_black = std::any_of(squares.begin(), squares.end(), std::bind(bishop_on_square_color, BLACK, _1));
    if(bishops_on_white && bishops_on_black)
    {
        return true;
    }

    auto knight_count = std::count_if(board.begin(), board.end(),
                                      [piece_is_right_color](auto piece)
                                      {
                                          return piece_is_right_color(piece) && piece.type() == KNIGHT;
                                      });
    return knight_count > 1 || (knight_count > 0 && (bishops_on_white || bishops_on_black));
}

void Board::set_thinking_mode(Thinking_Output_Type mode) noexcept
{
    thinking_indicator = mode;
}

Thinking_Output_Type Board::thinking_mode() noexcept
{
    return thinking_indicator;
}

void Board::pick_move_now() noexcept
{
    move_immediately = true;
}

void Board::choose_move_at_leisure() noexcept
{
    move_immediately = false;
}

bool Board::must_pick_move_now() noexcept
{
    return move_immediately;
}

void Board::update_board_hash(Square square) noexcept
{
    current_board_hash ^= square_hash(square);
}

void Board::update_whose_turn_hash() noexcept
{
    current_board_hash ^= switch_turn_board_hash;
}

uint64_t Board::square_hash(Square square) const noexcept
{
    assert(square.inside_board());

    auto piece = piece_on_square(square);
    auto index = square.index();
    auto result = square_hash_values[index][piece.index()];
    if(piece &&
       piece.type() == ROOK &&
       ! piece_has_moved(square) &&
       ! piece_has_moved(king_location[piece.color()]))
    {
        auto on_first_rank = (index%8 == 0);
        auto on_first_file = (index/8 == 0);
        result ^= castling_hash_values[2*on_first_file + on_first_rank];
    }

    if( ! piece && is_en_passant_targetable(square))
    {
        auto file_index = index/8;
        result = en_passant_hash_values[file_index];
    }

    return result;
}

uint64_t Board::board_hash() const noexcept
{
    return current_board_hash;
}

bool Board::move_captures(const Move& move) const noexcept
{
    auto attacked_piece = piece_on_square(move.end());

    // Assert move is actually legal
    assert(is_in_legal_moves_list(move));
    assert( ! attacked_piece || (move.can_capture() && attacked_piece.color() == opposite(whose_turn())));

    return attacked_piece || move.is_en_passant();
}

bool Board::move_changes_material(const Move& move) const noexcept
{
    return move_captures(move) || move.promotion_piece_symbol();
}

bool Board::king_multiply_checked() const noexcept
{
    return checking_moves().count() > 1;
}

bool Board::all_empty_between(Square start, Square end) const noexcept
{
    assert(straight_line_move(start, end));
    auto squares = Square::squares_between(start, end);
    return std::all_of(squares.begin(), squares.end(), [this](auto square) { return ! piece_on_square(square); });
}

bool Board::piece_is_pinned(Square square) const noexcept
{
    if(square == last_pin_check_square)
    {
        return last_pin_result;
    }
    last_pin_check_square = square;

    const auto& king_square = find_king(whose_turn());
    if(king_square == square)
    {
        return last_pin_result = false; // king is never pinned
    }

    if( ! straight_line_move(square, king_square))
    {
        return last_pin_result = false;
    }

    auto diff = king_square - square;
    if(potential_attacks[opposite(whose_turn())][square.index()][Move::attack_index(diff)])
    {
        // The potential_attacks check guarantees that there is an opposing piece attacking
        // the queried square in the same direction towards the friendly king. This next check
        // is to make sure the attacking piece is not a limited range piece--i.e., a pawn or king.
        auto attacker = piece_on_square(square - diff.step());
        return last_pin_result = ( ! attacker || (attacker.type() != PAWN && attacker.type() != KING)) &&
                                 all_empty_between(king_square, square);
    }
    else
    {
        return last_pin_result = false;
    }
}

void Board::add_board_position_to_repeat_record() noexcept
{
    add_to_repeat_count(board_hash());
}

void Board::add_to_repeat_count(uint64_t new_hash) noexcept
{
    repeat_count[repeat_count_insertion_point++] = new_hash;
}

ptrdiff_t Board::current_board_position_repeat_count() const noexcept
{
    return std::count(repeat_count.begin(),
                      repeat_count.begin() + repeat_count_insertion_point,
                      board_hash());
}

size_t Board::moves_since_pawn_or_capture() const noexcept
{
    return repeat_count_insertion_point - 1;
}

void Board::clear_repeat_count() noexcept
{
    repeat_count_insertion_point = 0;
}

size_t Board::castling_move_index(Color player) const noexcept
{
    return castling_index[player];
}

Board Board::without_random_pawn() const noexcept
{
    assert(std::any_of(board.begin(), board.end(), [](auto p) { return p && p.type() == PAWN; }));

    auto result = *this;
    while(true)
    {
        auto square = Square('a' + Random::random_integer(0, 7), Random::random_integer(1, 8));
        auto piece = result.piece_on_square(square);
        if(piece && piece.type() == PAWN)
        {
            result.remove_piece(square);
            return result;
        }
    }
}

std::vector<const Move*> Board::quiescent(const std::array<double, 6>& piece_values) const noexcept
{
    if(game_length() == 0)
    {
        return {};
    }

    // The capture_move at index i results in the state_value at index i + 1
    std::vector<const Move*> capture_moves;
    std::vector<double> state_values = {0.0};

    const auto player_color = whose_turn();
    auto current_board = *this;
    const auto square = previous_move->end();
    while( ! current_board.safe_for_king(square, opposite(current_board.whose_turn())))
    {
        std::vector<const Move*> capturing_moves;
        std::copy_if(current_board.legal_moves().begin(), current_board.legal_moves().end(), std::back_inserter(capturing_moves),
                        [square](auto move) { return move->end() == square; });

        if(capturing_moves.empty())
        {
            // This can happen when the only attacking piece is a king
            // and the attacked piece is guarded.
            break;
        }

        // Attack with the weakest piece first
        auto move = *std::min_element(capturing_moves.begin(), capturing_moves.end(),
                                      [&piece_values, &current_board](auto move1, auto move2)
                                      {
                                          return piece_values[current_board.piece_on_square(move1->start()).type()] <
                                                 piece_values[current_board.piece_on_square(move2->start()).type()];
                                      });

        // Make sure that an exchange does not lose material
        auto moving_piece = current_board.piece_on_square(move->start());
        auto attacked_piece = current_board.piece_on_square(move->end());
        current_board.submit_move(*move);
        capture_moves.push_back(move);
        state_values.push_back(state_values.back() + (moving_piece.color() == player_color ? +1 : -1)*piece_values[attacked_piece.type()]);
    }


    if(capture_moves.empty())
    {
        return capture_moves;
    }

    // Make sure to stop before either player ends up in an
    // avoidable loss of material.
    auto minimax_index = state_values.size() - 1;
    auto minimax_value = state_values[minimax_index];
    for(auto index = state_values.size() - 2; index < state_values.size(); --index)
    {
        auto value = state_values[index];

        // Even indices indicate a player's choice (maximize score).
        // Odd indices indicate an opponent's choice (minimize score).
        if(index % 2 == 0)
        {
            if(value > minimax_value)
            {
                minimax_value = value;
                minimax_index = index;
            }
        }
        else
        {
            if(value < minimax_value)
            {
                minimax_value = value;
                minimax_index = index;
            }
        }
    }

    return {capture_moves.begin(), capture_moves.begin() + minimax_index};
}

size_t Board::previous_moves_count() const noexcept
{
    return prior_moves_count;
}
