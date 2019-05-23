#include <iostream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cassert>
#include <stdexcept>
#include <array>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <cmath>

#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Square.h"
#include "Game/Game_Result.h"
#include "Game/Piece.h"

#include "Moves/Move.h"

#include "Players/Player.h"

#include "Exceptions/Illegal_Move.h"
#include "Exceptions/Promotion_Piece_Needed.h"

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
            for(auto& piece_indexed_random_int : square_indexed_row)
            {
                piece_indexed_random_int = Random::random_unsigned_int64();
            }
        }
        return hash_cache;
    }();

    const auto en_passant_hash_values =
    []()
    {
        // One entry for each file
        std::array<uint64_t, 8> en_passant_hash_cache;
        for(auto& file_indexed_random_int : en_passant_hash_cache)
        {
            file_indexed_random_int = Random::random_unsigned_int64();
        }
        return en_passant_hash_cache;
    }();

    const auto castling_hash_values =
    []()
    {
        // One entry for each rook
        std::array<uint64_t, 4> castling_hash_cache;
        for(auto& n : castling_hash_cache)
        {
            n = Random::random_unsigned_int64();
        }
        return castling_hash_cache;
    }();

    const uint64_t switch_turn_board_hash = Random::random_unsigned_int64();
}

const std::string Board::standard_starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//! Constructs a board according to an FEN string.
//
//! \param fen An optional text string given in FEN. If no argument is given,
//!        then the FEN string for the start of a standard chess game is given.
//! \throws std::invalid_argument Thrown if the FEN string does not represent a valid board state.
Board::Board(const std::string& fen) :
    board{},
    repeat_count_insertion_point{0},
    unmoved_positions{},
    starting_fen(String::remove_extra_whitespace(fen)),
    pinned_squares{},
    square_searched_for_pin{},
    checking_square{},
    potential_attacks{},
    castling_index{{size_t(-1), size_t(-1)}},
    attack_counts{},
    thinking_indicator(NO_THINKING)
{
    auto fen_parse = String::split(fen);
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
        for(auto symbol : board_parse.at(size_t(8-rank)))
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
        for(auto c : castling_parse)
        {
            if(c == '-')
            {
                fen_error("Castling section contains - and other characters: " + castling_parse + ".");
            }

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

    if(fen_status() != starting_fen)
    {
        fen_error("Result: " + fen_status());
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
}

void Board::fen_error(const std::string& reason) const
{
    throw std::invalid_argument("Bad FEN input: " + starting_fen + "\n" + reason);
}

Piece& Board::piece_on_square(Square square)
{
    return board[square.index()];
}

//! Get the piece on the square indicated by coordinates.
//
//! \param square The queried square.
//! \returns Pointer to piece on square. May be nullptr if square is emtpy.
Piece Board::piece_on_square(Square square) const
{
    return board[square.index()];
}

void Board::set_unmoved(Square square)
{
    update_board_hash(square); // remove reference to moved piece
    unmoved_positions[square.index()] = true;
    update_board_hash(square);
}

//! Checks if there are any legal moves from the start sqaure to the end square.
//
//! \param start The square where the move should start.
//! \param end   The square where the move should end.
//! \returns True if there is a legal move between the given squares.
bool Board::is_legal(Square start, Square end) const
{
    return std::any_of(legal_moves().begin(), legal_moves().end(),
                       [&](auto move)
                       {
                           return move->start() == start &&
                                  move->end() == end;
                       });
}

bool Board::is_in_legal_moves_list(const Move& move) const
{
    return std::find(legal_moves().begin(), legal_moves().end(), &move) != legal_moves().end();
}

//! Returns the status of the game in FEN.
//
//! \returns A textual representation of the game state.
//!
//! This may slightly differ from the output of other programs
//! in that the en passant target is only listed if there is a
//! legal en passant move to be made.
std::string Board::fen_status() const
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
                if( ! piece_has_moved({rook_file, base_rank})) // have rooks moved
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
        std::to_string(move_count_start_offset + game_record_listing.size()/2);
}

const Move& Board::create_move(Square start, Square end, char promote) const
{
    std::vector<const Move*> move_list;
    for(const auto& move : legal_moves())
    {
        if(move->start() == start &&
           move->end() == end)
        {
            if(promote)
            {
                if(move->promotion_piece_symbol() == std::toupper(promote))
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
        throw Illegal_Move("No legal move found for " +
                           start.string() +
                           "-" +
                           end.string() +
                           " (" + std::string(1, promote ? promote : ' ') + ")");
    }

    if(move_list.size() == 1)
    {
        return *move_list.front();
    }
    else
    {
        throw Promotion_Piece_Needed();
    }
}

//! Updates the state of the board according to a Player-selected Move.
//
//! \param move A Move-class instance. This must be an item taken from Board::legal_moves().
//!        The input move is only checked for legality in DEBUG builds, where illegal moves
//!        will trip an assert. In RELEASE builds, submitting an illegal move is undefined,
//!        resulting in anything from an invalid board state to a crash due to segfault.
//! \returns Returns a Game_Result indicating the result of the move and whether the game has ended.
Game_Result Board::submit_move(const Move& move)
{
    update_board(move);

    if(no_legal_moves())
    {
        if(king_is_in_check())
        {
            auto winner = opposite(whose_turn());
            return Game_Result(winner, color_text(winner) + " mates");
        }
        else
        {
            return Game_Result(NONE, "Stalemate");
        }
    }

    // An insufficient material draw can only happen after a capture
    // or a pawn promotion to a minor piece, both of which clear the
    // repeat_count tracker.
    if(moves_since_pawn_or_capture() == 0 && ! enough_material_to_checkmate())
    {
        return Game_Result(NONE, "Insufficient material");
    }

    if(current_board_position_repeat_count() >= 3)
    {
        return Game_Result(NONE, "Threefold repetition");
    }

    // "Move" means both players move, so the fifty-move rule is
    // triggered after 100 player moves
    if(moves_since_pawn_or_capture() >= 100)
    {
        return Game_Result(NONE, "50-move limit");
    }

    return {};
}

void Board::update_board(const Move& move)
{
    assert(is_in_legal_moves_list(move));

    game_record_listing.push_back(&move);
    move_piece(move);
    move.side_effects(*this);

    switch_turn();

    add_board_position_to_repeat_record();
}

//! Creates a Move instance given a text string representation.
//
//! \param move A string using coordinate notation ("a2b3") or PGN ("Bb3").
//! \returns A Move instance corresponding to the input string.
//! \throws Illegal_Move if the text does not represent a legal move or if the wanted move is ambiguous.
const Move& Board::create_move(const std::string& move) const
{
    static const std::string pieces = "RNBQK";
    static const std::string valid_files = "abcdefgh";
    static const std::string valid_rows = "12345678";
    static const std::string castling = "O";
    static const std::string valid_characters = pieces + valid_files + valid_rows + castling;

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
        throw Illegal_Move(move + " does not specify a valid move.");
    }

    // Castling
    if(validated == "OO")
    {
        return create_move({'e', whose_turn() == WHITE ? 1 : 8},
                           {'g', whose_turn() == WHITE ? 1 : 8});
    }
    if(validated == "OOO")
    {
        return create_move({'e', whose_turn() == WHITE ? 1 : 8},
                           {'c', whose_turn() == WHITE ? 1 : 8});
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
            throw Illegal_Move("Illegal text move (wrong length): " + move);
        }

        auto start = Square{move[0], move[1] - '0'};
        auto end   = Square{move[2], move[3] - '0'};
        if(move.size() == 5)
        {
            promoted_piece = std::toupper(move[4]);
        }

        return create_move(start, end, promoted_piece);
    }

    // else normal PGN-style piece movement
    auto ending_square = Square{validated[validated.size() - 2], validated[validated.size() - 1] - '0'};
    if( ! ending_square.inside_board())
    {
        throw Illegal_Move("Illegal text move (out of board): " + move);
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

void Board::move_piece(const Move& move)
{
    if(piece_on_square(move.end()))
    {
        remove_piece(move.end());
        clear_repeat_count();
    }

    auto moving_piece = piece_on_square(move.start());
    remove_piece(move.start());
    place_piece(moving_piece, move.end());

    clear_en_passant_target();
    clear_pinned_squares();
    clear_checking_square();
}

//! Tells which player is due to move.
//
//! \returns Color of player who is next to move.
Color Board::whose_turn() const
{
    return turn_color;
}

//! Get a list of all legal moves for the current player.
//
//! \returns A list of pointers to legal moves. Any call to Board::submit_move() must take
//!          its argument from this list.
const std::vector<const Move*>& Board::legal_moves() const
{
    return legal_moves_cache;
}

//! Prints an ASCII version of the board to a terminal.
//
//! This is useful for Human_Player on the terminal and debugging.
//! \param perspective Specifies which side of the board is at the bottom of the screen.
void Board::ascii_draw(Color perspective) const
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

void Board::remove_piece(Square square)
{
    place_piece({}, square);
}

void Board::adjust_attack_counts_common(Piece piece1, Piece piece2, Square square, int adding_or_substracting)
{
    assert(std::abs(adding_or_substracting) == 1);

    // piece1 is the piece for which the attacks are being adjusted based
    // upon the precense of piece2. The Freedom To Move Gene does not count
    // attacks on a piece of the same color as an attack. So, if a piece of
    // the same color is removed, the attacked square is added to the count.
    // If a piece is placed on a square that is attacked by the same color,
    // then the attack count for that color is decrimented.
    if(piece1 && ( ! piece2 || piece2.color() != piece1.color()))
    {
        attack_counts[piece1.color()] += adding_or_substracting*moves_attacking_square(square, piece1.color()).count();
    }
}

void Board::adjust_attack_counts(Piece removed_piece, Piece moved_piece, Square square)
{
    adjust_attack_counts_common(removed_piece, moved_piece, square,  1);
    adjust_attack_counts_common(moved_piece, removed_piece, square, -1);
}

void Board::place_piece(Piece piece, Square square)
{
    update_board_hash(square); // XOR out piece on square

    auto old_piece = piece_on_square(square);
    piece_on_square(square) = piece;

    remove_attacks_from(square, old_piece);
    update_blocks(square, old_piece, piece);
    add_attacks_from(square, piece);
    adjust_attack_counts(old_piece, piece, square);

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

    // Make sure the attack count did not drop below zero.
    // Unsigned wraparound would result in an absurdly large
    // number, so check against an impossible number of attacks.
    // Namely, the sum of all squares being attacked from every
    // possible direction.
    assert(std::all_of(attack_counts.begin(), attack_counts.end(), [](auto n) { return n <= 16*64; }));
}

//! Returns the number of attacking moves available.
//
//! The method is used in the Freedom_To_Move_Gene::score_board() method.
//! \param attacking_color The color of pieces doing the attacking.
//! \returns The number of attacking moves excepting those that attack pieces
//!          of the same color.
size_t Board::attack_count(Color attacking_color) const
{
    return attack_counts[attacking_color];
}

void Board::add_attacks_from(Square square, Piece piece)
{
    modify_attacks(square, piece, true);
}

void Board::modify_attacks(Square square, Piece piece, bool adding_attacks)
{
    if( ! piece)
    {
        return;
    }

    auto attacking_color = piece.color();
    auto vulnerable_king = Piece{opposite(attacking_color), KING};
    const Move* blocked_move = nullptr;
    auto attack_count_diff = adding_attacks ? 1 : -1;
    for(auto attack : piece.attacking_moves(square))
    {
        auto attacked_square = attack->end();
        auto attacked_index = attacked_square.index();

        if(blocked_move && same_direction(blocked_move->movement(), attack->movement()))
        {
            blocked_attacks[attacking_color][attacked_index][attack->attack_index()] = adding_attacks;
        }
        else
        {
            blocked_move = nullptr;
            auto blocking_piece = piece_on_square(attacked_square);

            potential_attacks[attacking_color][attacked_index][attack->attack_index()] = adding_attacks;
            if( ! blocking_piece || blocking_piece.color() != attacking_color)
            {
                attack_counts[attacking_color] += attack_count_diff;
            }

            if(blocking_piece && blocking_piece != vulnerable_king)
            {
                blocked_move = attack;
            }
        }
    }
}

void Board::remove_attacks_from(Square square, Piece old_piece)
{
    modify_attacks(square, old_piece, false);
}

void Board::update_blocks(Square square, Piece old_piece, Piece new_piece)
{
    // Replacing nothing with nothing changes nothing
    if( ! old_piece && ! new_piece)
    {
        return;
    }

    // Replacing one piece with another does not change which
    // moves are blocked. Only happens during pawn promotions.
    if(old_piece && new_piece)
    {
        return;
    }

    auto add_new_attacks = ! new_piece; // New pieces block; no new pieces allow new moves through
    auto attack_count_diff = add_new_attacks ? 1 : -1;
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

                for(auto target_square = square + step; target_square.inside_board(); target_square += step)
                {
                    auto target_index = target_square.index();
                    auto piece = piece_on_square(target_square);
                    if(( ! piece || piece.color() != attacking_color) && (potential_attacks[attacking_color][target_index][index] != add_new_attacks))
                    {
                        attack_counts[attacking_color] += attack_count_diff;
                    }

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

//! Get a list of all moves that attack a given square.
//
//! \param square The square being attacked.
//! \param attacking_color The color of pieces doing the attacking.
const std::bitset<16>& Board::moves_attacking_square(Square square, Color attacking_color) const
{
    return potential_attacks[attacking_color][square.index()];
}

const std::bitset<16>& Board::checking_moves() const
{
    return moves_attacking_square(find_king(whose_turn()), opposite(whose_turn()));
}

//! Find out if the king of the player to move is currently in check.
//
//! \returns If the current player is in check.
bool Board::king_is_in_check() const
{
    return checking_moves().any();
}

//! Find out whether the input square is safe for the given king to occupy.
//
//! A square is not safe for the king if the opposing pieces can attack the square.
//! \param square The queried square.
//! \param king_color The color of the king piece that is under potential attack.
//! \returns Whether the king would be in check if it was placed on the square in question.
bool Board::safe_for_king(Square square, Color king_color) const
{
    return moves_attacking_square(square, opposite(king_color)).none();
}

//! Determine if there are any attacks on a square that are blocked by other pieces.
//
//! \param square The queried square.
//! \param attacking_color The color of the attacking pieces.
//! \returns Whether there is an attack on the square that is blocked by another piece.
bool Board::blocked_attack(Square square, Color attacking_color) const
{
    return blocked_attacks[attacking_color][square.index()].any();
}

//! Check if a move will leave the player making the move in check.
//
//! \param move A possibly legal move to check.
//! \returns Whether the move under consideration will leave the friendly king in check.
bool Board::king_is_in_check_after_move(const Move& move) const
{
    if(piece_on_square(move.start()).type() == KING)
    {
        return ! safe_for_king(move.end(), whose_turn());
    }

    if(king_is_in_check())
    {
        // King is under check from multiple pieces but not moving
        if(king_multiply_checked())
        {
            return true; // moving a piece can only block one checking piece
        }

        // Checking piece is captured by non-pinned piece
        if( ! checking_square.is_set())
        {
            const auto& checks = checking_moves();
            size_t checking_index = 0;
            while( ! checks[checking_index])
            {
                ++checking_index;
            }
            auto step = Move::attack_direction_from_index(checking_index);
            const auto& king_square = find_king(whose_turn());

            checking_square = king_square - step;
            while( ! piece_on_square(checking_square))
            {
                checking_square -= step;
            }
        }

        if(checking_square == move.end())
        {
            return piece_is_pinned(move.start());
        }

        // Non-pinned piece moves to block check
        if( ! piece_on_square(move.end()))
        {
            if(piece_is_pinned(move.end()))
            {
                return piece_is_pinned(move.start());
            }
        }

        // En passant capture of checking pawn
        if(move.is_en_passant())
        {
            auto pawn_square = Square{move.end().file(), move.start().rank()};
            return checking_square != pawn_square || piece_is_pinned(move.start());
        }

        // Nothing is done about the check
        return true;
    }

    if(piece_is_pinned(move.start()))
    {
        const auto& king_square = find_king(whose_turn());

        auto pin_direction = king_square - move.start();

        // If move direction and pin direction are not parallel, the piece will move
        // out of pin and expose the king to check.
        return ! moves_are_parallel(move.movement(), pin_direction);
    }

    if(move.is_en_passant())
    {
        // Check if king is on same row as both pawns and there is an opposing rook or queen on row
        const auto& king_square = find_king(whose_turn());
        if(king_square.rank() != move.start().rank())
        {
            return false;
        }

        auto step = (move.start() - king_square).step();
        auto attacking_file = move.start().file();
        auto attacked_file = move.end().file();
        auto passed_pawns = 0;
        for(auto square = king_square + step; square.inside_board(); square += step)
        {
            if(square.file() == attacked_file || square.file() == attacking_file)
            {
                ++passed_pawns;
                continue;
            }

            auto piece = piece_on_square(square);
            if(piece)
            {
                if(passed_pawns < 2)
                {
                    return false;
                }

                if(piece.color() == whose_turn())
                {
                    return false;
                }

                return piece.type() == ROOK || piece.type() == QUEEN;
            }
        }
    }

    return false;
}

bool Board::no_legal_moves() const
{
    return legal_moves().empty();
}

//! Get the history of moves.
//
//! \returns The list of moves made on this board.
const std::vector<const Move*>& Board::game_record() const
{
    return game_record_listing;
}

template<typename OutputStream, typename DataType>
void print_game_header_line(OutputStream& output, const std::string& heading, const DataType& data)
{
    output << "[" << heading << " \"" << data << "\"]\n";
}

//! Prints the PGN game record with commentary from Players.
//
//! \param white Pointer to Player playing white to provide commentary for moves. Can be nullptr.
//! \param black Pointer to Player playing black to provide commentary for moves. Can be nullptr.
//! \param file_name Name of the text file where the game will be printed. If empty, print to stdout.
//! \param result The result of the last action (move, clock punch, or outside intervention) in the game.
//! \param game_clock The game clock used during the game.
void Board::print_game_record(const Player* white,
                              const Player* black,
                              const std::string& file_name,
                              const Game_Result& result,
                              const Clock& game_clock) const
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

    auto game_start_time = game_clock.game_start_date_and_time();
    auto game_start_time_c = std::chrono::system_clock::to_time_t(game_start_time);
    std::tm time_out;
#ifdef _WIN32
    localtime_s(&time_out, &game_start_time_c);
#elif defined(__linux__)
    localtime_r(&game_start_time_c, &time_out);
#endif
    print_game_header_line(out_stream, "Date", std::put_time(&time_out, "%Y.%m.%d"));
    print_game_header_line(out_stream, "Time", std::put_time(&time_out, "%H:%M:%S"));

    if(white && ! white->name().empty())
    {
        print_game_header_line(out_stream, "White", white->name());
    }

    if(black && ! black->name().empty())
    {
        print_game_header_line(out_stream, "Black", black->name());
    }

    print_game_header_line(out_stream, "Round", game_number++);

    if(game_clock.initial_time() > 0)
    {
        std::ostringstream time_control_spec;
        if(game_clock.moves_to_reset() > 0)
        {
            time_control_spec << game_clock.moves_to_reset() << '/';
        }
        time_control_spec << game_clock.initial_time();
        if(game_clock.increment() > 0)
        {
            time_control_spec << '+' << game_clock.increment();
        }
        print_game_header_line(out_stream, "TimeControl", time_control_spec.str());
        print_game_header_line(out_stream, "TimeLeftWhite", game_clock.time_left(WHITE));
        print_game_header_line(out_stream, "TimeLeftBlack", game_clock.time_left(BLACK));
    }

    if( ! result.game_ending_annotation().empty())
    {
        print_game_header_line(out_stream, "Result", result.game_ending_annotation());
    }

    if( ! result.ending_reason().empty() && ! String::contains(result.ending_reason(), "mates"))
    {
        print_game_header_line(out_stream, "Termination", result.ending_reason());
    }

    auto temp = Board(starting_fen);
    if(starting_fen != standard_starting_fen)
    {
        print_game_header_line(out_stream, "SetUp", 1);
        print_game_header_line(out_stream, "FEN", starting_fen);
    }

    auto starting_turn_offset = size_t(temp.whose_turn() == WHITE ? 0 : 1);

    for(size_t i = 0; i < game_record_listing.size(); ++i)
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

        auto next_move = game_record_listing.at(i);
        out_stream << " " << next_move->game_record_item(temp);

        auto current_player = (temp.whose_turn() == WHITE ? white : black);
        if(current_player)
        {
            auto commentary = String::trim_outer_whitespace(current_player->commentary_for_next_move(temp));
            if( ! commentary.empty())
            {
                out_stream << " { " << commentary << " }";
            }
        }

        temp.submit_move(*next_move);
    }
    out_stream << "\n\n\n";
}

//! Prints the last move of the game in coordinate notation.
//
//! Used to communicate with external chess programs. This is an altnative to
//! Board::last_move_record() that doesn't need to replay the game to create
//! the PGN notation move.
//! \returns Coordinate-style move string ("e2e4").
std::string Board::last_move_coordinates() const
{
    return game_record_listing.back()->coordinate_move();
}

void Board::switch_turn()
{
    turn_color = opposite(turn_color);
    update_whose_turn_hash();
    recreate_move_caches();
}

void Board::make_en_passant_targetable(Square square)
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

//! Determine whether the indicated square can be a target of an en passant move.
//
//! The is called by En_Passant::move_specific_legal().
//! \param square The queried square.
//! \returns Whether the square was passed by a pawn double move.
bool Board::is_en_passant_targetable(Square square) const
{
    return en_passant_target == square;
}

void Board::clear_en_passant_target()
{
    make_en_passant_targetable({});
}

void Board::clear_pinned_squares()
{
    square_searched_for_pin.fill(false);
}

void Board::clear_checking_square()
{
    checking_square = Square{};
}

//! Indicates whether the queried square has a piece on it that never moved.
//
//! \param square The queried squaer.
//! \returns If the piece on the square has never moved during the game.
bool Board::piece_has_moved(Square square) const
{
    return ! unmoved_positions[square.index()];
}

//! Finds the square on which a king resides.
//
//! \param color Which king to find.
//! \returns Square which contains the sought after king.
Square Board::find_king(Color color) const
{
    assert(piece_on_square(king_location[color]) == Piece(color, KING));
    return king_location[color];
}

void Board::recreate_move_caches()
{
    legal_moves_cache.clear();

    bool en_passant_legal = false;
    material_change_move_available = false;
    for(auto square : Square::all_squares())
    {
        auto piece = piece_on_square(square);
        if(piece && piece.color() == whose_turn())
        {
            // Invalid direction
            const Move* blocked_move = nullptr;

            for(const auto move : piece.move_list(square))
            {
                auto blocked = blocked_move && same_direction(move->movement(), blocked_move->movement());

                if( ! blocked)
                {
                    blocked_move = nullptr;
                    if(move->is_legal(*this))
                    {
                        legal_moves_cache.push_back(move);

                        en_passant_legal = en_passant_legal || move->is_en_passant();
                        material_change_move_available = material_change_move_available ||
                                                         move_captures(*move) ||
                                                         move->promotion_piece_symbol();
                    }

                    // If there is a piece on the ending square, farther moves are not possible.
                    // The check for a promotion piece is needed since the set of pawn promotions
                    // results in moves with identical beginning and ending squares, which would
                    // result in the first pawn-promotion-by-capture blocking all the others.
                    if(piece_on_square(move->end()) && ! move->promotion_piece_symbol())
                    {
                        blocked_move = move;
                    }
                }
            }
        }
    }

    // Make sure board hash is correct for valid repeat counts
    if( ! en_passant_legal)
    {
        clear_en_passant_target();
    }
}

//! Checks whether there are enough pieces on the board for any possible checkmate.
//
//! The following piece sets on the board make checkmate possible:
//! - Any single pawn, rook, or queen,
//! - At least two bishops (of any color) on oppositely colored squares,
//! - A bishop and knight (of an color combination),
//! - Two knights (of any color combination).
//! \param color If NONE, check both sides for enough material. Otherwise, only check the pieces of one side.
//! \returns If there are enough pieces on the board to make a checkmate arrangement.
//!          If the method returns false when called with NONE, this will usually lead to a drawn game.
bool Board::enough_material_to_checkmate(Color color) const
{
    bool knight_found = false;
    Color bishop_square_color_found = NONE;

    for(auto square : Square::all_squares())
    {
        auto piece = piece_on_square(square);
        if( ! piece)
        {
            continue;
        }

        if(color != NONE && piece.color() != color)
        {
            continue;
        }

        auto type = piece.type();
        if(type == KING)
        {
            continue;
        }

        if(type == QUEEN || type == ROOK || type == PAWN)
        {
            return true; // checkmate possible with just queen or rook; pawn can promote
        }

        // Newly found piece is either a bishop or knight
        if(knight_found)
        {
            return true; // checkmate with a knight and any non-king piece on either side is possible
        }

        if(type == KNIGHT)
        {
            if(bishop_square_color_found != NONE)
            {
                return true;
            }
            knight_found = true;
        }
        else // BISHOP
        {
            auto bishop_square_color = square.color();
            if(bishop_square_color_found == NONE)
            {
                bishop_square_color_found = bishop_square_color;
            }
            else if(bishop_square_color != bishop_square_color_found)
            {
                return true; // checkmate with opposite colored bishops possible
            }
        }
    }

    return false;
}

//! Get last move for dispaly in text-based UIs.
//
//! \returns Last move on this board in PGN notation
std::string Board::last_move_record() const
{
    Board b(starting_fen);
    std::string result;
    for(const auto& move : game_record())
    {
        result = move->game_record_item(b);
        b.submit_move(*move);
    }

    return result;
}

//! Set the format an engine should output while picking a move.
//
//! \param mode Which chess engine protocol is being used: CECP or NO_THINKING.
void Board::set_thinking_mode(Thinking_Output_Type mode) const
{
    thinking_indicator = mode;
}

//! Find out what kind of format an engine should output while picking a move.
//
//! \returns Format of thinking output: CECP or NO_THINKING.
Thinking_Output_Type Board::thinking_mode() const
{
    return thinking_indicator;
}

void Board::update_board_hash(Square square)
{
    current_board_hash ^= square_hash(square);
}

void Board::update_whose_turn_hash()
{
    current_board_hash ^= switch_turn_board_hash;
}

uint64_t Board::square_hash(Square square) const
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

//! Returns the Zobrist hash of the current state of the board.
//
//! See https://en.wikipedia.org/wiki/Zobrist_hashing for details.
uint64_t Board::board_hash() const
{
    return current_board_hash;
}

//! Report if the last move change the pieces on the board.
//
//! This is more general than Board::last_move_captured() since it also checks
//! for pawn promotions.
//! \returns Whether the last move captured or resulted in a promoted pawn.
bool Board::last_move_changed_material() const
{
    return last_move_captured() ||
           ( ! game_record_listing.empty() && game_record_listing.back()->promotion_piece_symbol());
}

//! Report if the most recent move in the game captured a piece.
//
//! This method is used by chess engines to decide how interesting a board position is
//! and how much time to devote to future moves.
//! \returns Whether the last move captured an opponent's piece.
bool Board::last_move_captured() const
{
    if(game_record_listing.empty() || moves_since_pawn_or_capture() > 0)
    {
        return false;
    }

    auto last_move = game_record_listing.back();
    if(piece_on_square(last_move->end()).type() == PAWN ||
       last_move->promotion_piece_symbol())
    {
        // Check if move was pawn capture
        return last_move->file_change() != 0;
    }

    // Not a pawn move and moves_since_pawn_or_capture() == 0 ==> capturing move
    return true;
}

//! Check if any currently legal move changes material by capturing or promoting a pawn.
//
//! This method is used by chess engines to determine how interesting a board position
//! is and thus how much time to devote to studying future moves.
//! \returns If any legal move captures or promotes a pawn.
bool Board::material_change_possible() const
{
    return material_change_move_available;
}

//! Determines whether a move captures on the current board.
//
//! \param move Move to check.
//! \returns If the move captures an opponent's piece.
//! \throws assertion_failure In DEBUG builds, if the move to check is not legal, an assert fails.
bool Board::move_captures(const Move& move) const
{
    auto attacked_piece = piece_on_square(move.end());

    // Assert move is actually legal
    assert(is_in_legal_moves_list(move));
    assert( ! attacked_piece || (move.can_capture() && attacked_piece.color() == opposite(whose_turn())));

    return attacked_piece || move.is_en_passant();
}

bool Board::king_multiply_checked() const
{
    return checking_moves().count() > 1;
}

//! Check whether all of the squares between two squares are empty.
//
//! This method assumes the squares are along the same rank, file, or diagonal.
//! \param start The first square.
//! \param end   The second square.
//! \returns Whether all of the intervening squares between the two input squares are unoccupied by pieces.
//! \throws assertion_failure In DEBUG builds, squares not along the same rank, file, or diagonal
//!         will trigger an assertion failure. In RELEASE builds, the result is undefined.
bool Board::all_empty_between(Square start, Square end) const
{
    assert(straight_line_move(start, end));

    auto step = (end - start).step();

    for(auto square = start + step; square != end; square += step)
    {
        assert(square.inside_board());

        if(piece_on_square(square))
        {
            return false;
        }
    }

    return true;
}

//! Determine whether a piece would be pinned to the king by an opposing piece if it was on the given square.
//
//! \param square The queried square.
//! \returns Whether there is an opposing piece (of color opposite(Board::whose_turn()))
//!          that would pin a piece occupying the queried square to its king. For example,
//!          if it is White's turn, is there a Black piece that would pin a (possibly non-existant)
//!          white piece to the white king.
bool Board::piece_is_pinned(Square square) const
{
    auto index = square.index();
    if(square_searched_for_pin[index])
    {
        return pinned_squares[index];
    }

    square_searched_for_pin[index] = true;
    auto& pin_result = pinned_squares[index];

    const auto& king_square = find_king(whose_turn());

    if(king_square == square)
    {
        return pin_result = false; // king is never pinned
    }

    if( ! straight_line_move(square, king_square))
    {
        return pin_result = false;
    }

    auto diff = king_square - square;
    if(potential_attacks[opposite(whose_turn())][square.index()][Move::attack_index(diff)])
    {
        // The potential_attacks check guarantees that there is an opposing piece attacking
        // the queried square in the same direction towards the friendly king. This next check
        // is to make sure the attacking piece is not a limited range piece--i.e., a pawn or king.
        auto attacker = piece_on_square(square - diff.step());
        return pin_result = ( ! attacker || (attacker.type() != PAWN && attacker.type() != KING)) &&
                            all_empty_between(king_square, square);
    }
    else
    {
        return pin_result = false;
    }
}

void Board::add_board_position_to_repeat_record()
{
    add_to_repeat_count(board_hash());
}

void Board::add_to_repeat_count(uint64_t new_hash)
{
    repeat_count[repeat_count_insertion_point++] = new_hash;
}

ptrdiff_t Board::current_board_position_repeat_count() const
{
    return std::count(repeat_count.begin(),
                      repeat_count.begin() + repeat_count_insertion_point,
                      board_hash());
}

//! The number of moves since the last capture or pawn move.
//
//! \returns How many moves have been made since the last capturing or pawn move.
size_t Board::moves_since_pawn_or_capture() const
{
    return repeat_count_insertion_point - 1;
}

void Board::clear_repeat_count()
{
    repeat_count_insertion_point = 0;
}

//! Gets the ply move during which a player castled.
//
//! \param player The color of the player being queried.
//! \returns The ply count when the player castled, or MAX(size_t) if the player has not castled.
size_t Board::castling_move_index(Color player) const
{
    return castling_index[player];
}

//! Create a copy of the board with a random pawn removed.
//
//! \throws Debug assertion failure if there are no pawns on the board.
Board Board::without_random_pawn() const
{
    assert(std::any_of(board.begin(), board.end(), [](auto p) { return p && p.type() == PAWN; }));

    auto result = *this;
    while(true)
    {
        auto square = Square(Random::random_integer('a', 'h'), Random::random_integer(1, 8));
        auto piece = result.piece_on_square(square);
        if(piece && piece.type() == PAWN)
        {
            result.remove_piece(square);
            return result;
        }
    }
}
